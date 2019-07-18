aux_source_directory (. SRC_LIST)
include_directories (
	"../include/"
	"../include/msgpack"
	"../include/rest_rpc"
	"../include/nanolog"
	"../include/doctest"
	"/opt/boost_1_69_0"
)
link_directories (${PROJECT_SOURCE_DIR}/libs)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -ltcmalloc -pthread -std=c++17")
add_executable (zeg_robot_simulator ${SRC_LIST})
set (EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
target_link_libraries (zeg_robot_simulator stdc++fs)
option(BUILD_TESTS "If enabled, compile the tests." ON)
if(BUILD_TESTS)
        enable_testing()
        macro(add_doctest test_source test_name)
        add_executable(${test_name} ${test_source})
        add_test(${test_name} ${test_name})
        endmacro(add_doctest)
        add_subdirectory(tests)
endif()
