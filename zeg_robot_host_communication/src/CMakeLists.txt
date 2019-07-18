aux_source_directory (. SRC_LIST)
include_directories (
	"../include/doctest"
	"../include/zmq"
	"../include/libevent"
	"../include/nanolog"
	"../include/thread_pool"
	"../include/rest_rpc"
	"/opt/boost_1_69_0"
	"../include/msgpack"
	"../include"
)
link_directories (${PROJECT_SOURCE_DIR}/libs)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -lrt -levent -ltcmalloc -pthread -std=c++17")
add_executable (zeg_robot_host_communication ${SRC_LIST})
set (EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
target_link_libraries (zeg_robot_host_communication zmq event stdc++fs)
option(BUILD_TESTS "If enabled, compile the tests." ON)
if(BUILD_TESTS)
        enable_testing()
        macro(add_doctest test_source test_name)
        add_executable(${test_name} ${test_source})
        add_test(${test_name} ${test_name})
        endmacro(add_doctest)
        add_subdirectory(tests)
endif()
