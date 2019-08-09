#ifndef SRC_UDP_BROADCAST_CLIENT_HPP_
#define SRC_UDP_BROADCAST_CLIENT_HPP_
#include "message_communicate_entity_maker.hpp"
class udp_broadcast_client : public message_communicate_entity {
public:
	udp_broadcast_client() {
		sock_fd_ = -1;
		port_ = 7780;
		broadcast_address_ = "255.255.255.255";
		memset(&dest_addr_, 0, sizeof(dest_addr_));
		memset(&client_addr_, 0, sizeof(client_addr_));
		make_dest_addr();
	}
	~udp_broadcast_client() {
		if (sock_fd_ >= 0) {
			close(sock_fd_);
		}
	}
public:
	inline bool init() override {
		return init_sock_fd();
	}
	inline bool init_sock_fd() {
		sock_fd_ = socket(AF_INET, SOCK_DGRAM, 0);
		if (sock_fd_ < 0) {
			return false;
		}
		int so_broadcast = 1;
		return setsockopt(sock_fd_, SOL_SOCKET, SO_BROADCAST, (char *)&so_broadcast, sizeof(so_broadcast)) >= 0;
	}
	inline void set_port(int port) {
		port_ = port;
		make_dest_addr();
	}
	inline void set_broadcast_address(const char *address) {
		broadcast_address_ = address;
		make_dest_addr();
	}
	virtual int send(const char *buf, int len) override {
		return sendto(sock_fd_, buf, len, 0, (struct sockaddr *)&dest_addr_, sizeof(dest_addr_));
	}
	virtual int recv(char *recv_buf, int buf_len) override {
		if (buf_len <= 0 || buf_len > BUF_SIZE) {
			return -1;
		}
		char buf[BUF_SIZE] = "";
		socklen_t size = sizeof(struct sockaddr);
		int len = recvfrom(sock_fd_, buf, sizeof(buf), 0, (struct sockaddr *)&client_addr_, &size);
		if (len <= BUF_SIZE && len > 0) {
			len = min(len, buf_len);
			memcpy(recv_buf, buf, len);
		}
		return len;
	}
private:
	inline void make_dest_addr() {
		dest_addr_.sin_family = AF_INET;
		dest_addr_.sin_port = htons(port_);
		dest_addr_.sin_addr.s_addr = inet_addr(broadcast_address_);
	}
private:
	int sock_fd_;
	int port_;
	const char *broadcast_address_;
	struct sockaddr_in dest_addr_;
	struct sockaddr_in client_addr_;
private:
	const int BUF_SIZE = 1024;
};
REGISTER_MESSAGE_COMMUNICATE_ENTITY(udp_broadcast_client, "udp.broadcast.client");

#endif /* SRC_UDP_BROADCAST_CLIENT_HPP_ */
