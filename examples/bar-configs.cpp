/**
 * This program dumps all available barconfigs
 */

#include <iostream>

#include <i3ipc++/ipc.hpp>


static void  dump_bar_config(const i3ipc::bar_config_t&  bc) {
	std::cout << '"' << bc.id << '"' << std::endl
		<< "\tmode = " << static_cast<char>(bc.mode) << std::endl
		<< "\tposition = " << static_cast<char>(bc.position) << std::endl
		<< "\tstatus_command = \"" << bc.status_command << '"' << std::endl
		<< "\tfont = \"" << bc.font << '"' << std::endl
		<< "\tworkspace_buttons = " << (bc.workspace_buttons ? "true" : "false") << std::endl
		<< "\tbinding_mode_indicator = " << (bc.binding_mode_indicator ? "true" : "false") << std::endl
		<< "\tverbose = " << (bc.verbose ? "true" : "false") << std::endl
		<< "\tcolors:" << std::endl;

	std::cout << std::hex;
	for (auto  iter = bc.colors.begin(); iter != bc.colors.end(); iter++) {
		std::cout << "\t\t\"" << iter->first << "\" = #" << iter->second << std::endl;
	}
	std::cout << std::dec;
}


int  main() {
	// First of all needs to create a connection
	i3ipc::connection  conn;

	// Then request a list of barconfigs
	std::vector<std::string>  bar_configs = conn.get_bar_configs_list();

	// And dump 'em all!!!!!
	for (auto&  name : bar_configs) {
		std::shared_ptr<i3ipc::bar_config_t>  bc = conn.get_bar_config(name);
		dump_bar_config(*bc);
	}

	return 0;
}
