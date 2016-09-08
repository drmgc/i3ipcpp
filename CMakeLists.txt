cmake_minimum_required(VERSION 3.0 FATAL_ERROR)
project(i3ipc++)

string(TIMESTAMP I3IPCppBUILD_DATETIME "%Y-%m-%d %H:%M:%S")

option(I3IPCpp_WITH_TESTS "Build unit tests executables" OFF)
option(I3IPCpp_BUILD_EXAMPLES "Build example executables" OFF)

find_package(PkgConfig)
pkg_check_modules(SIGCPP REQUIRED sigc++-2.0)
pkg_check_modules(JSONCPP REQUIRED jsoncpp)


set(I3IPCpp_INCLUDE_DIRS
	${SIGCPP_INCLUDE_DIRS}
	${i3ipc++_SOURCE_DIR}/3rd/auss/include
	${i3ipc++_SOURCE_DIR}/include/
)

set(I3IPCpp_LIBRARY_DIRS
	${SIGCPP_LIBRARY_DIRS}
	${JSONCPP_LIBRARY_DIRS}
	${PROJECT_BINARY_DIR}
)

set(I3IPCpp_LIBRARIES
	${SIGCPP_LIBRARIES}
	${JSONCPP_LIBRARIES}
)


# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -Wextra -Wno-unused-parameter")
# set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g3 -DDEBUG")
# set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O2")

file(GLOB_RECURSE SRC src/*.cpp)
add_library(i3ipc++ ${SRC})

target_include_directories(i3ipc++
	PUBLIC
		${I3IPCpp_INCLUDE_DIRS}
	PRIVATE
		${JSONCPP_INCLUDE_DIRS}
		${i3ipc++_SOURCE_DIR}/include/i3ipc++
)

link_directories(i3ipc++
	PUBLIC
		${I3IPCpp_LIBRARY_DIRS}
	PRIVATE
		${JSONCPP_LIBRARY_DIRS}
)

target_link_libraries(i3ipc++
	PUBLIC
		${I3IPCpp_LIBRARIES}
)

target_compile_options(i3ipc++
	PUBLIC -std=c++11 -Wall -Wextra -Wno-unused-parameter
)

target_compile_definitions(i3ipc++
	PRIVATE I3IPC_BUILD_DATETIME="${I3IPCppBUILD_DATETIME}"
)

if (CMAKE_BUILD_TYPE EQUAL "DEBUG")
	target_compile_options(i3ipc++
		PUBLIC -g3
	)
	target_compile_definitions(i3ipc++
		PRIVATE DEBUG=1
	)
elseif (CMAKE_BUILD_TYPE EQUAL "RELEASE")
	target_compile_options(i3ipc++
		PUBLIC -O2
	)
endif()

# set(I3IPCpp_LIBRARIES i3ipc++ ${SIGCPP_LIBRARIES} ${JSONCPP_LIBRARIES})

set(I3IPCpp_LIBRARY_DIRS "${I3IPCpp_LIBRARY_DIRS} ${PROJECT_BINARY_DIR}")
set(I3IPCpp_LIBRARIES "${I3IPCpp_LIBRARIES};i3ipc++")

get_directory_property(HAS_PARENT_DIRECTORY PARENT_DIRECTORY)
if(HAS_PARENT_DIRECTORY)
	set(I3IPCpp_LIBRARY_DIRS ${I3IPCpp_LIBRARY_DIRS} PARENT_SCOPE)
	set(I3IPCpp_INCLUDE_DIRS ${I3IPCpp_INCLUDE_DIRS} PARENT_SCOPE)
	set(I3IPCpp_LIBRARIES ${I3IPCpp_LIBRARIES} PARENT_SCOPE)
endif()

if(I3IPCpp_BUILD_EXAMPLES)
	add_subdirectory(${i3ipc++_SOURCE_DIR}/examples)
endif()

if(I3IPCpp_WITH_TESTS)
	find_package(CxxTest)
	if(CXXTEST_FOUND)
		add_definitions(
			-DTEST_SRC_ROOT="${i3ipc++_SOURCE_DIR}/test"
		)

		enable_testing()
		file(GLOB SRC_TEST test/*.hpp)
		CXXTEST_ADD_TEST(i3ipcpp_check test.cpp ${SRC_TEST})
		target_compile_options(i3ipcpp_check
			PUBLIC -std=c++11 -Wall -Wextra -Wno-unused-parameter -g3
		)
		target_compile_definitions(i3ipcpp_check
			PRIVATE DEBUG=1
		)

		target_include_directories(i3ipcpp_check
			PUBLIC
				${I3IPCpp_INCLUDE_DIRS}
			PRIVATE
				${JSONCPP_INCLUDE_DIRS}
				${i3ipc++_SOURCE_DIR}/include/i3ipc++
		)

		link_directories(i3ipcpp_check
			PUBLIC
				${I3IPCpp_LIBRARY_DIRS}
		)

		target_link_libraries(i3ipcpp_check
			PUBLIC
				${I3IPCpp_LIBRARIES}
		)
	else()
		message(WARNING "CxxTest not found. Unable to run unit-tests")
	endif()
endif()
