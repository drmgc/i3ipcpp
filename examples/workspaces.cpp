#include <iostream>

#include <i3ipc++/ipc.hpp>

int  main() {
	i3ipc::I3Connection  conn;
	for (auto&  w : conn.get_workspaces()) {
		std::cout << '#' << std::hex << w.num << std::dec
			<< "\n\tName: " << w.name
			<< "\n\tVisible: " << w.visible
			<< "\n\tFocused: " << w.focused
			<< "\n\tUrgent: " << w.urgent
			<< "\n\tRect: "
			<< "\n\t\tX: " << w.rect.x
			<< "\n\t\tY: " << w.rect.y
			<< "\n\t\tWidth: " << w.rect.width
			<< "\n\t\tHeight: " << w.rect.height
			<< "\n\tOutput: " << w.output
			<< std::endl;
	}
	return 0;
}
