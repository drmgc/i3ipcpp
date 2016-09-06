/**
 * This program dumps a tree of windows and workspaces to console
 */

#include <iostream>

#include <i3ipc++/ipc.hpp>


/**
 * Reqursively dump containers of a tree
 * @param  c      a root container
 * @param  prefix an alignment
 */
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
	// First of all needs to create a connection
	i3ipc::connection  conn;

	// Then we dump workspaces
	for (auto&  w : conn.get_workspaces()) {
		std::cout << '#' << std::hex << w->num << std::dec
			<< "\n\tName: " << w->name
			<< "\n\tVisible: " << w->visible
			<< "\n\tFocused: " << w->focused
			<< "\n\tUrgent: " << w->urgent
			<< "\n\tRect: "
			<< "\n\t\tX: " << w->rect.x
			<< "\n\t\tY: " << w->rect.y
			<< "\n\t\tWidth: " << w->rect.width
			<< "\n\t\tHeight: " << w->rect.height
			<< "\n\tOutput: " << w->output
			<< std::endl;
	}

	// Then we dump the tree
	std::string  prefix_buf;
	dump_tree_container(*conn.get_tree(), prefix_buf);

	return 0;
}
