#ifndef SRC_UDP_UNICAST_SERVER_HPP_
#define SRC_UDP_UNICAST_SERVER_HPP_
#include "message_communicate_entity_maker.hpp"
class udp_unicast_server : public message_communicate_entity {
public:
	udp_unicast_server() {
		sock_fd_ = -1;
		port_ = 7780;
		memset(&client_addr_, 0, sizeof(client_addr_));
	}
	~udp_unicast_server() {
		if (sock_fd_ >= 0) {
			close(sock_fd_);
		}
	}
public:
	inline bool init() override {
		return init_sock_fd() && bind_sock_fd() && set_timeout();
	}
	inline bool init_sock_fd() {
		sock_fd_ = socket(AF_INET, SOCK_DGRAM, 0);
		return sock_fd_ >= 0;
	}
	inline bool bind_sock_fd() {
		struct sockaddr_in addr = {0};
		addr.sin_family = AF_INET;
		addr.sin_addr.s_addr = htonl(INADDR_ANY);
		addr.sin_port = htons(port_);
		return bind(sock_fd_, (struct sockaddr *)&addr, sizeof(addr)) >= 0;
	}
	inline void set_port(int port) {
		port_ = port;
	}
	inline bool set_timeout(int s = 5) {
		struct timeval timeout;
		timeout.tv_sec = s;
		timeout.tv_usec = 0;
		return setsockopt(sock_fd_, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) >= 0;
	}
	virtual int send(const char *buf, int len) override {
		return sendto(sock_fd_, buf, len, 0, (struct sockaddr *)&client_addr_, sizeof(client_addr_));
	}
	virtual int recv(char *recv_buf, int buf_len) override {
		if (buf_len <= 0 || buf_len > BUFSIZ) {
			return -1;
		}
		char buf[BUFSIZ] = "";
		socklen_t size = sizeof(struct sockaddr);
		int len = recvfrom(sock_fd_, buf, sizeof(buf), 0, (struct sockaddr *)&client_addr_, &size);
		if (len <= BUFSIZ && len > 0) {
			len = min(len, buf_len);
			memcpy(recv_buf, buf, len);
		}
		return len;
	}
private:
	int sock_fd_;
	int port_;
	struct sockaddr_in client_addr_;
};
REGISTER_MESSAGE_COMMUNICATE_ENTITY(udp_unicast_server, "udp.unicast.server");

#endif /* SRC_UDP_UNICAST_SERVER_HPP_ */
