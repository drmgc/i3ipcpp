#include <iostream>

#include <i3ipc++/ipc.hpp>


void  dump_tree_container(const i3ipc::container_t&  c, std::string&  prefix) {
	std::cout << prefix << "ID: " << c.id << " (i3's; X11's - " << c.xwindow_id << ")" << std::endl;
	prefix.push_back('\t');
	std::cout << prefix << "name = \"" << c.name << "\"" << std::endl;
	std::cout << prefix << "type = \"" << c.type << "\"" << std::endl;
	std::cout << prefix << "border = \"" << c.border_raw << "\"" << std::endl;
	std::cout << prefix << "current_border_width = " << c.current_border_width << std::endl;
	std::cout << prefix << "layout = \"" << c.layout_raw << "\"" << std::endl;
	std::cout << prefix << "percent = " << c.percent << std::endl;
	if (c.urgent) {
		std::cout << prefix << "urgent" << std::endl;
	}
	if (c.focused) {
		std::cout << prefix << "focused" << std::endl;
	}
	prefix.push_back('\t');
	for (auto&  n : c.nodes) {
		dump_tree_container(*n, prefix);
	}
	prefix.pop_back();
	prefix.pop_back();
}


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
	std::string  prefix_buf;
	dump_tree_container(*conn.get_tree(), prefix_buf);

	return 0;
}
