#ifndef UDP_BROADCAST_AGENT_HPP_
#define UDP_BROADCAST_AGENT_HPP_
#include "message_communicate_agent.hpp"
class udp_broadcast_agent : public message_communicate_agent {
public:
	udp_broadcast_agent() {
		sock_fd_ = -1;
		port_ = 17789;
	}
	~udp_broadcast_agent() {
		if (sock_fd_ >= 0) {
			close(sock_fd_);
		}
	}
public:
	bool init() override {
		return init_sock_fd() && bind_sock_fd();
	}
	bool init_sock_fd() {
		sock_fd_ = socket(AF_INET, SOCK_DGRAM, 0);
		if (sock_fd_ < 0) {
			return false;
		}
		int so_broadcast = 1;
		return setsockopt(sock_fd_, SOL_SOCKET, SO_BROADCAST, (char *)&so_broadcast, sizeof(so_broadcast)) >= 0;
	}
	bool bind_sock_fd() {
		struct sockaddr_in addr = {0};
		addr.sin_family = AF_INET;
		addr.sin_addr.s_addr = htonl(INADDR_ANY);
		addr.sin_port = htons(port_);
		return bind(sock_fd_, (struct sockaddr *)&addr, sizeof(addr)) >= 0;
	}
	void set_port(int port) {
		port_ = port;
	}
	virtual int send_broadcast(const char *buf, int len, const char *broadcast_address) override {
		struct sockaddr_in dest_addr = {0};
		dest_addr.sin_family = AF_INET;
		dest_addr.sin_port = htons(port_);
		dest_addr.sin_addr.s_addr = inet_addr(broadcast_address);
		int res = sendto(sock_fd_, buf, len, 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
		return res;
	}
	virtual int recv_broadcast(char *recv_buf) override {
		char buf[BUFSIZ] = "";
		socklen_t size = sizeof(struct sockaddr);
		struct sockaddr_in client_addr = {0};
		int len = recvfrom(sock_fd_, buf, sizeof(buf), 0, (struct sockaddr *)&client_addr, &size);
		cout << "connection port = " << client_addr.sin_port << endl;
		cout << "connection ip = " << inet_ntoa(client_addr.sin_addr) << endl;
		strncpy(recv_buf, buf, strlen(buf));
		return len;
	}
	virtual int send_unicast(const char *buf, int len, const char *broadcast_address) override {
		return 0;
	}
	virtual int recv_unicast(char *recv_buf) override {
		return 0;
	}
private:
	int sock_fd_;
	int port_;
};

#endif /* UDP_BROADCAST_AGENT_HPP_ */
