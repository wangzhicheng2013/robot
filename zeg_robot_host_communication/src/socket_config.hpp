#ifndef SRC_SOCKET_CONFIG_HPP_
#define SRC_SOCKET_CONFIG_HPP_
#include <string>
using namespace std;
enum {
	TCP,
	UDP
};
class socket_config {
public:
	socket_config() {
		port_ = 7780;
		backlog_ = 1024;
		type_ = UDP;
	}
	virtual ~socket_config() = default;
public:
	int port_;
	int backlog_;
	unsigned char type_;
};

#endif /* SRC_SOCKET_CONFIG_HPP_ */
