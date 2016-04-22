#pragma once

#include <cstdint>
#include <string>
#include <memory>
#include <vector>

#ifdef USE_FULL_SIGNALS
#include <json/json.h>
#endif

#include <sigc++/sigc++.h>

extern "C" {
#include <i3/ipc.h>
}

/**
 * @addtogroup i3ipc i3 IPC C++ binding
 * @{
 */
namespace i3ipc {

/**
 * Get path to the i3 IPC socket
 * @return Path to a socket
 */
std::string  get_socketpath();

/**
 * Primitive of rectangle
 */
struct rect_t {
	int  x; /**< Position on X axis */
	int  y; /**< Position on Y axis */
	int  width; /**< Width of rectangle */
	int  height; /**< Height of rectangle */
};

/**
 * i3's workspace
 */
struct workspace_t {
	int  num; /**< Index of the worksapce */
	std::string  name; /**< Name of the workspace */
	bool  visible; /**< Is the workspace visible */
	bool  focused; /**< Is the workspace is currently focused */
	bool  urgent; /**< Is the workspace is urgent */
	rect_t  rect; /**< A size of the workspace */
	std::string  output; /**< An output of the workspace */
};

/**
 * i3's output
 */
struct output_t {
	std::string  name; /**< Name of the output */
	bool  active; /**< Is the output currently active */
	std::string  current_workspace; /**< Name of current workspace */
	rect_t  rect; /**< Size of the output */
};

/**
 * Version of i3
 */
struct version_t {
	std::string  human_readable; /**< Human redable version string */
	std::string  loaded_config_file_name; /**< Path to current config of i3 */
	uint32_t  major; /**< Major version of i3 */
	uint32_t  minor; /**< Minor version of i3 */
	uint32_t  patch; /**< Patch number of i3 */
};

/**
 * Types of the events of i3
 */
enum EventType {
	ET_WORKSPACE = (1 << 0), /**< Workspace event */
	ET_OUTPUT = (1 << 1), /**< Output event */
	ET_MODE = (1 << 2), /**< Output mode event */
	ET_WINDOW = (1 << 3), /**< Window event */
	ET_BARCONFIG_UPDATE = (1 << 4), /**< Bar config update event @attention Yet is not implemented as signal in I3Connection */
};

#ifndef USE_FULL_SIGNALS
/**
 * Types of workspace events
 */
enum class WorkspaceEventType : char {
	FOCUS = 'f', /**< Focused */
	INIT = 'i', /**< Initialized */
	EMPTY = 'e', /**< Became empty */
	URGENT = 'u', /**< Became urgent */
};

/**
 * Types of window events
 */
enum class WindowEventType : char {
	NEW = 'n', /**< Window created */
	CLOSE = 'c', /**< Window closed */
	FOCUS = 'f', /**< Window got focus */
	TITLE = 't', /**< Title of window has been changed */
	FULLSCREEN_MODE = 'F', /**< Window toggled to fullscreen mode */
	MOVE = 'M', /**< Window moved */
	FLOATING = '_', /**< Window toggled floating mode */
	URGENT = 'u', /**< Window became urgent */
};
#endif

struct buf_t;
/**
 * Connection to the i3
 */
class I3Connection {
public:
	/**
	 * Connect to the i3
	 * @param  socket_path path to a i3 IPC socket
	 */
	I3Connection(const std::string&  socket_path = get_socketpath());
	~I3Connection();

	/**
	 * Send a command to i3
	 * @param  command command
	 * @return         Is command successfully executed
	 */
	bool  send_command(const std::string&  command) const;

	/**
	 * Request a list of workspaces
	 * @return List of workspaces
	 */
	std::vector<workspace_t>  get_workspaces() const;

	/**
	 * Request a list of outputs
	 * @return List of outputs
	 */
	std::vector<output_t>  get_outputs() const;

	/**
	 * Request a version of i3
	 * @return Version of i3
	 */
	version_t  get_version() const;

	/**
	 * Subscribe on an events of i3
	 * 
	 * If connection isn't handling events at the moment, event numer will be added to subscription list.
	 * Else will also send subscripe request to i3
	 *
	 * Example:
	 * @code{.cpp}
	 * I3Connection  conn;
	 * conn.subscribe(i3ipc::ipc::ET_WORKSPACE | i3ipc::ipc::ET_WINDOW);
	 * @endcode
	 * 
	 * @param  events event type (EventType enum)
	 * @return        Is successfully subscribed. If connection isn't handling events at the moment, then always true.
	 */
	bool  subscribe(const int32_t  events);

	/**
	 * Prepare connection to the handling of i3's events
	 * @note Used only in main()
	 */
	void  prepare_to_event_handling();

	/**
	 * Handle an event from i3
	 * @note Used only in main()
	 */
	void  handle_event();

	sigc::signal<void>  signal_output_event; /**< Output event signal */
	sigc::signal<void>  signal_mode_event; /**< Output mode event signal */
	sigc::signal<void>  signal_barconfig_update_event; /**< Barconfig update event signal */
#ifdef USE_FULL_SIGNALS
	sigc::signal<void, const Json::Value&>  signal_workspace_event; /**< Workspace event signal */
	sigc::signal<void, const Json::Value&>  signal_window_event; /**< Window event signal */
#else
	sigc::signal<void, WorkspaceEventType>  signal_workspace_event; /**< Workspace event signal */
	sigc::signal<void, WindowEventType>  signal_window_event; /**< Window event signal */
#endif
	sigc::signal<void, EventType, const std::shared_ptr<const buf_t>&>  signal_event; /**< i3 event signal @note Default handler routes event to signal according to type */
private:
	const int32_t  m_main_socket;
	int32_t  m_event_socket;
	int32_t  m_subscriptions;
	const std::string  m_socket_path;
};

}

/**
 * @}
 */
