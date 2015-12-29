#include <cstdio>
#include <cstring>
#include <stdexcept>
#include <iostream>

#include <auss.hpp>
#include <json/json.h>

#include "log.hpp"
#include "ipc-util.hpp"
#include "ipc.hpp"

namespace i3ipc {

// For log.hpp
std::vector<std::ostream*>  g_logging_outs = {
	&std::cout,
};
std::vector<std::ostream*>  g_logging_err_outs = {
	&std::cerr,
};

#define IPC_JSON_READ(ROOT) \
	{ \
		Json::Reader  reader; \
		if (!reader.parse(std::string(buf->payload, buf->header->size), ROOT, false)) { \
			throw invalid_reply_payload_error(auss_t() << "Failed to parse reply on \"" i3IPC_TYPE_STR "\": " << reader.getFormattedErrorMessages()); \
		} \
	}

#define IPC_JSON_ASSERT_TYPE(OBJ, OBJ_DESCR, TYPE_CHECK, TYPE_NAME) \
	{\
		if (!(OBJ).TYPE_CHECK()) { \
			throw invalid_reply_payload_error(auss_t() << "Failed to parse reply on \"" i3IPC_TYPE_STR "\": " OBJ_DESCR " expected to be " TYPE_NAME); \
		} \
	}
#define IPC_JSON_ASSERT_TYPE_OBJECT(OBJ, OBJ_DESCR) IPC_JSON_ASSERT_TYPE(OBJ, OBJ_DESCR, isObject, "an object")
#define IPC_JSON_ASSERT_TYPE_ARRAY(OBJ, OBJ_DESCR) IPC_JSON_ASSERT_TYPE(OBJ, OBJ_DESCR, isArray, "an array")
#define IPC_JSON_ASSERT_TYPE_BOOL(OBJ, OBJ_DESCR) IPC_JSON_ASSERT_TYPE(OBJ, OBJ_DESCR, isBool, "a bool")
#define IPC_JSON_ASSERT_TYPE_INT(OBJ, OBJ_DESCR) IPC_JSON_ASSERT_TYPE(OBJ, OBJ_DESCR, isInt, "an integer")


inline rect_t  parse_rect_from_json(const Json::Value&  value) {
	return {
		.x = value["x"].asInt(),
		.y = value["y"].asInt(),
		.width = value["width"].asInt(),
		.height = value["height"].asInt(),
	};
}


std::string  get_socketpath() {
	std::string  str;
	{
		auss_t  str_buf;
		FILE*  in;
		char  buf[512] = {0};
		if (!(in = popen("i3 --get-socketpath", "r"))) {
			throw std::runtime_error("Failed to get socket path");
		}

		while (fgets(buf, sizeof(buf), in) != nullptr) {
			str_buf << buf;
		}
		pclose(in);
		str = str_buf;
	}
	if (str.back() == '\n') {
		str.pop_back();
	}
	return str;
}


I3Connection::I3Connection(const std::string&  socket_path) : m_main_socket(i3_connect(socket_path)), m_event_socket(-1), m_subscriptions(0), m_socket_path(socket_path) {
#define i3IPC_TYPE_STR "i3's event"
	signal_event.connect([this](EventType  event_type, const std::shared_ptr<const buf_t>&  buf) {
		switch (event_type) {
		case ET_WORKSPACE: {
			WorkspaceEventType  type;
			Json::Value  root;
			IPC_JSON_READ(root);
			std::string  change = root["change"].asString();
			if (change == "focus") {
				type = WorkspaceEventType::FOCUS;
			} else if (change == "init") {
				type = WorkspaceEventType::INIT;
			} else if (change == "empty") {
				type = WorkspaceEventType::EMPTY;
			} else if (change == "urgent") {
				type = WorkspaceEventType::URGENT;
			} else {
				I3IPC_WARN("Unknown workspace event type " << change)
				break;
			}
			I3IPC_DEBUG("WORKSPACE " << change)

			signal_workspace_event.emit(type);
			break;
		}
		case ET_OUTPUT:
			I3IPC_DEBUG("OUTPUT")
			signal_output_event.emit();
			break;
		case ET_MODE:
			I3IPC_DEBUG("MODE")
			signal_mode_event.emit();
			break;
		case ET_WINDOW: {
			WindowEventType  type;
			Json::Value  root;
			IPC_JSON_READ(root);
			std::string  change = root["change"].asString();
			if (change == "new") {
				type = WindowEventType::NEW;
			} else if (change == "close") {
				type = WindowEventType::CLOSE;
			} else if (change == "focus") {
				type = WindowEventType::FOCUS;
			} else if (change == "title") {
				type = WindowEventType::TITLE;
			} else if (change == "fullscreen_mode") {
				type = WindowEventType::FULLSCREEN_MODE;
			} else if (change == "move") {
				type = WindowEventType::MOVE;
			} else if (change == "floating") {
				type = WindowEventType::FLOATING;
			} else if (change == "urgent") {
				type = WindowEventType::URGENT;
			}
			I3IPC_DEBUG("WINDOW " << change)

			signal_window_event.emit(type);
			break;
		}
		case ET_BARCONFIG_UPDATE:
			I3IPC_DEBUG("BARCONFIG_UPDATE")
			signal_barconfig_update_event.emit();
			break;
		};
	});
#undef i3IPC_TYPE_STR
}
I3Connection::~I3Connection() {
	i3_disconnect(m_main_socket);
	if (m_event_socket > 0)
		i3_disconnect(m_event_socket);
}


void  I3Connection::prepare_to_event_handling() {
	m_event_socket = i3_connect(m_socket_path);
	this->subscribe(m_subscriptions);
}
void  I3Connection::handle_event() {
	if (m_event_socket <= 0) {
		throw std::runtime_error("event_socket_fd <= 0");
	}
	auto  buf = i3_recv(m_event_socket);

	this->signal_event.emit(static_cast<EventType>(1 << (buf->header->type & 0x7f)), std::static_pointer_cast<const buf_t>(buf));
}


bool  I3Connection::subscribe(const int32_t  events) {
#define i3IPC_TYPE_STR "SUBSCRIBE"
	if (m_event_socket <= 0) {
		m_subscriptions |= events;
		return true;
	}
	std::string  payload;
	{
		auss_t  payload_auss;
		if (events & static_cast<int32_t>(ET_WORKSPACE)) {
			payload_auss << "\"workspace\",";
		}
		if (events & static_cast<int32_t>(ET_OUTPUT)) {
			payload_auss << "\"output\",";
		}
		if (events & static_cast<int32_t>(ET_MODE)) {
			payload_auss << "\"mode\",";
		}
		if (events & static_cast<int32_t>(ET_WINDOW)) {
			payload_auss << "\"window\",";
		}
		if (events & static_cast<int32_t>(ET_BARCONFIG_UPDATE)) {
			payload_auss << "\"barconfig_update\",";
		}
		payload = payload_auss;
		if (payload.empty()) {
			return true;
		}
		payload.pop_back();
	}
	I3IPC_DEBUG("i3 IPC subscriptions: " << payload)

	auto  buf = i3_msg(m_event_socket, ClientMessageType::SUBSCRIBE, auss_t() << '[' << payload << ']');
	Json::Value  root;
	IPC_JSON_READ(root)

	m_subscriptions |= events;

	return root["success"].asBool();
#undef i3IPC_TYPE_STR
}


version_t  I3Connection::get_version() const {
#define i3IPC_TYPE_STR "GET_VERSION"
	auto  buf = i3_msg(m_main_socket, ClientMessageType::GET_VERSION);
	Json::Value  root;
	IPC_JSON_READ(root)
	IPC_JSON_ASSERT_TYPE_OBJECT(root, "root")

	return {
		.human_readable = root["human_readable"].asString(),
		.loaded_config_file_name = root["loaded_config_file_name"].asString(),
		.major = root["major"].asUInt(),
		.minor = root["minor"].asUInt(),
		.patch = root["patch"].asUInt(),
	};
#undef i3IPC_TYPE_STR
}


std::vector<output_t>  I3Connection::get_outputs() const {
#define i3IPC_TYPE_STR "GET_OUTPUTS"
	auto  buf = i3_msg(m_main_socket, ClientMessageType::GET_OUTPUTS);
	Json::Value  root;
	IPC_JSON_READ(root)
	IPC_JSON_ASSERT_TYPE_ARRAY(root, "root")

	std::vector<output_t>  outputs;

	for (auto w : root) {
		Json::Value  name = w["name"];
		Json::Value  active = w["active"];
		Json::Value  current_workspace = w["current_workspace"];
		Json::Value  rect = w["rect"];

		outputs.push_back({
			.name = name.asString(),
			.active = active.asBool(),
			.rect = parse_rect_from_json(rect),
			.current_workspace = (current_workspace.isNull() ? std::string() : current_workspace.asString()),
		});
	}

	return outputs;
#undef i3IPC_TYPE_STR
}


std::vector<workspace_t>  I3Connection::get_workspaces() const {
#define i3IPC_TYPE_STR "GET_WORKSPACES"
	auto  buf = i3_msg(m_main_socket, ClientMessageType::GET_WORKSPACES);
	Json::Value  root;
	IPC_JSON_READ(root)
	IPC_JSON_ASSERT_TYPE_ARRAY(root, "root")

	std::vector<workspace_t>  workspaces;

	for (auto w : root) {
		Json::Value  num = w["num"];
		Json::Value  name = w["name"];
		Json::Value  visible = w["visible"];
		Json::Value  focused = w["focused"];
		Json::Value  urgent = w["urgent"];
		Json::Value  rect = w["rect"];
		Json::Value  output = w["output"];

		workspaces.push_back({
			.num = num.asInt(),
			.name = name.asString(),
			.visible = visible.asBool(),
			.focused = focused.asBool(),
			.urgent = urgent.asBool(),
			.rect = parse_rect_from_json(rect),
			.output = output.asString(),
		});
	}

	return workspaces;
#undef i3IPC_TYPE_STR
}


bool  I3Connection::send_command(const std::string&  command) const {
#define i3IPC_TYPE_STR "COMMAND"
	auto  buf = i3_msg(m_main_socket, ClientMessageType::COMMAND, command);
	Json::Value  root;
	IPC_JSON_READ(root)
	IPC_JSON_ASSERT_TYPE_ARRAY(root, "root")
	Json::Value  payload = root[0];
	IPC_JSON_ASSERT_TYPE_OBJECT(payload, " first item of root")

	if (payload["success"].asBool()) {
		return true;
	} else {
		Json::Value  error = payload["error"];
		if (!error.isNull()) {
			I3IPC_ERR("Failed to execute command: " << error.asString())
		}
		return false;
	}
#undef i3IPC_TYPE_STR
}

}
