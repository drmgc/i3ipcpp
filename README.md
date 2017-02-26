[![License](http://img.shields.io/:license-mit-blue.svg)](http://doge.mit-license.org)

i3ipc++
=======
An implementation of i3 IPC in C++11.

## Requirements

* cmake (>= 3.0)
* C++11 compiler
* sigc++ 2.0
* jsoncpp

## Using
Yet the only way of using is to add this repo as a submodule

```bash
git submodule add https://github.com/drmgc/i3ipcpp.git ./i3ipc++/
```

Then just type this in your `CMakeLists.txt`:

```cmake
...
add_subdirectory(i3ipc++)

include_directories(${I3IPCpp_INCLUDE_DIRS})
link_directories(${I3IPCpp_LIBRARY_DIRS})
...
```

And then just link:

```cmake
...
target_link_libraries(someapp ${I3IPCpp_LIBRARIES})
...
```

## Usage

See also examples in `examples/` directory.

### Connecting

```c++
#include <i3ipc++/ipc.hpp>

i3ipc::connection  conn;
```

The connection will be established automaticly.

### Event handling

First of all you need to declare the events you want to handle. As example we want to handle an binding and workspace events:
```c++
conn.subscribe(i3ipc::ET_WORKSPACE | i3ipc::ET_BINDING);
```

Then we need to connect to the signal handlers:
```c++
// Handler of WORKSPACE EVENT
conn.signal_workspace_event.connect([](const i3ipc::workspace_event_t&  ev) {
	std::cout << "workspace_event: " << (char)ev.type << std::endl;
	if (ev.current) {
		std::cout << "\tSwitched to #" << ev.current->num << " - \"" << ev.current->name << '"' << std::endl;
	}
});

// Handler of binding event
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
```

Then we starting the event-handling loop
```c++
while (true) {
	conn.handle_event();
}
```

**Note:** If you want to interract with event_socket or just want to prepare manually you can call `conn.connect_event_socket()` (if you want to reconnect `conn.connect_event_socket(true)`), but if by default `connect_event_socket()` called on first `handle_event()` call.

### Requesting

Also you can request some data from i3, as example barconfigs:
```c++
std::vector<std::string>  bar_configs = conn.get_bar_configs_list();
```

And then do with them something:
```c++
for (auto&  name : bar_configs) {
	std::shared_ptr<i3ipc::bar_config_t>  bc = conn.get_bar_config(name);

	// ... handling
}
```

### Sending commands

And, of course, you can command i3:
```c++
if (!conn.send_command("exit")) {
	throw std::string("Failed to exit via command");
}
```

## Version i3 support
It is written according to the *current* specification, so some of new features in IPC can be not-implemented. If there is some of them, please notice at issues page.

## Documentation
The latest documentation you can find [**here**](http://drmgc.github.io/docs/api-ref/i3ipc++/latest/)

## Licensing
This library is licensed under under the MIT license, but it also uses [`JsonCpp`](https://github.com/open-source-parsers/jsoncpp) (*only for parsing i3's replies*) and my header-only library [`auss`](https://github.com/drmgc/auss)

## Backward compatibility note
While version is `0.x` there can be a lack of backward compatibility between minor releases, please see release notes.
