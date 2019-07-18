cmake_minimum_required(VERSION 3.7)
project(zeg_mock_rest_rpc)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -std=c++11")
set (EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

include_directories(
	"../include/rest_rpc"
	"/opt/boost_1_69_0"
       	"../include/msgpack"
)

add_executable(zeg_mock_rest_rpc main.cpp)
target_link_libraries(zeg_mock_rest_rpc)

