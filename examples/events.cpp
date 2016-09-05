#include <iostream>

#include <i3ipc++/ipc.hpp>


int  main() {
	i3ipc::connection  conn;
	conn.subscribe(i3ipc::ET_WORKSPACE | i3ipc::ET_WINDOW | i3ipc::ET_BINDING);

	conn.signal_workspace_event.connect([](const i3ipc::workspace_event_t&  ev) {
		std::cout << "workspace_event: " << (char)ev.type << std::endl;
	});
	conn.signal_window_event.connect([](const i3ipc::window_event_t&  ev) {
		std::cout << "window_event: " << (char)ev.type << std::endl;
	});
	conn.signal_binding_event.connect([](const i3ipc::binding_t&  b) {
		std::cout << "binding_event:" << std::endl
			<< "\tcommand = \"" << b.command << '"' << std::endl
			<< "\tinput_code = " << b.input_code << std::endl
			<< "\tsymbol = " << b.symbol << std::endl
			<< "\tinput_type = " << static_cast<char>(b.input_type) << std::endl
			<< "\tevent_state_mask =" << std::endl;
		for (const std::string& s : b.event_state_mask) {
			std::cout << "\t\t\"" << s << '"' << std::endl;
		}
	});

	while (true) {
		conn.handle_event();
	}

	return 0;
}
