[![License](http://img.shields.io/:license-mit-blue.svg)](http://doge.mit-license.org)

i3ipc++
=======
An implementation of i3 IPC in C++11.

## Requirements

* C++11 compiler
* sigc++ 2.0

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

## Version i3 support
It is written according to the *current* specification, so some of new features in IPC can be implemented. If there is some of them, please notice at issues page.

## Examples
There is few examples in `examples/` directory.

## Documentation
The latest documentation you can find [**here**](http://drmgc.github.io/docs/api-ref/i3ipc++/latest/)

## Licensing
This library is licensed under under the MIT license, but it also uses [`JsonCpp`](https://github.com/open-source-parsers/jsoncpp) (*only for parsing i3's replies*) and my library [`auss`](https://github.com/drmgc/auss)
