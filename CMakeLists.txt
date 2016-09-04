cmake_minimum_required(VERSION 3.0)
project(i3ipc++)

option(I3IPCpp_WITH_TESTS "Build unit tests executables" OFF)
option(I3IPCpp_BUILD_EXAMPLES "Build example executables" OFF)

find_package(PkgConfig)
pkg_check_modules(SIGCPP REQUIRED sigc++-2.0)
pkg_check_modules(JSONCPP REQUIRED jsoncpp)

include_directories(
	${SIGCPP_INCLUDE_DIRS}
	${JSONCPP_INCLUDE_DIRS}
	3rd/auss/include
	include/i3ipc++
)

link_directories(
	${SIGCPP_LIBRARY_DIRS}
	${JSONCPP_LIBRARY_DIRS}
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -Wextra -Wno-unused-parameter")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g3 -DDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O2")

file(GLOB_RECURSE SRC src/*.cpp)
add_library(i3ipc++_static STATIC ${SRC})

set(I3IPCpp_LIBRARY_DIRS ${CMAKE_CURRENT_BINARY_DIR})
set(I3IPCpp_INCLUDE_DIRS
	${SIGCPP_INCLUDE_DIRS}
	3rd/auss/include
	${CMAKE_CURRENT_SOURCE_DIR}/include/
)
set(I3IPCpp_LIBRARIES i3ipc++_static ${SIGCPP_LIBRARIES} ${JSONCPP_LIBRARIES})

set(I3IPCpp_LIBRARY_DIRS ${I3IPCpp_LIBRARY_DIRS} PARENT_SCOPE)
set(I3IPCpp_INCLUDE_DIRS ${I3IPCpp_INCLUDE_DIRS} PARENT_SCOPE)
set(I3IPCpp_LIBRARIES ${I3IPCpp_LIBRARIES} PARENT_SCOPE)

if(I3IPCpp_BUILD_EXAMPLES)
	add_subdirectory(examples)
endif()

if(I3IPCpp_WITH_TESTS)
	find_package(CxxTest)
	if(CXXTEST_FOUND)
		include_directories(${CXXTEST_INCLUDE_DIR})
		include_directories(src/)
		add_definitions(
			-DTEST_SRC_ROOT="${CMAKE_CURRENT_SOURCE_DIR}/test"
		)

		enable_testing()
		file(GLOB SRC_TEST test/*.hpp)
		CXXTEST_ADD_TEST(i3ipcpp_check test.cpp ${SRC_TEST})
		target_link_libraries(i3ipcpp_check ${I3IPCpp_LIBRARIES})
	else()
		message(WARNING "CxxTest not found. Unable to run unit-tests")
	endif()
endif()
