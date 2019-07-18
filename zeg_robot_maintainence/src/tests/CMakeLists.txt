add_doctest(unit_test.cpp unit_test)
include_directories (
	"../"
	"/opt/boost_1.69.0"
)
target_link_libraries (unit_test zmq stdc++fs)
