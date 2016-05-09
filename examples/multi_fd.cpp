#include <iostream>

extern "C" {
#include <unistd.h>
}

#include <i3ipc++/ipc.hpp>


int  main() {
	i3ipc::connection  conn;
	conn.subscribe(i3ipc::ET_WORKSPACE | i3ipc::ET_WINDOW);
	conn.add_fd(STDIN_FILENO).connect([](){
		char input[4096] = {0, };
		if ( fgets(input, sizeof(input), stdin) == NULL)
			return;
		std::cout << "Text input: " << input;
	});

	conn.signal_workspace_event.connect([](const i3ipc::workspace_event_t&  ev) {
		std::cout << "workspace_event: " << (char)ev.type << std::endl;
	});
	conn.signal_window_event.connect([](const i3ipc::window_event_t&  ev) {
		std::cout << "window_event: " << (char)ev.type << std::endl;
	});

	// Don't forget this:
	conn.prepare_to_event_handling();

	while (true) {
		conn.handle_event();
	}

	return 0;
}