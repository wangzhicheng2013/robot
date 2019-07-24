add_doctest(unit_test.cpp unit_test)
include_directories (
	"../"
	"/opt/boost_1.69.0"
)
set(CMAKE_CXX_LINK_EXECUTABLE "${CMAKE_CXX_LINK_EXECUTABLE} -ldl")
target_link_libraries (unit_test zmq event stdc++fs)
