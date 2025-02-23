#ifndef UDP_UNICAST_AGENT_HPP_
#define UDP_UNICAST_AGENT_HPP_
#include "message_communicate_agent.hpp"
class udp_unicast_agent : public message_communicate_agent {
public:
	udp_unicast_agent() {
		sock_fd_ = -1;
		port_ = 17790;
		memset(&client_addr_, 0, sizeof(client_addr_));
	}
	~udp_unicast_agent() {
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
		return sock_fd_ >= 0;
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
	virtual int send_unicast(const char *buf, int len, const char *unicast_address) override {
		struct sockaddr_in dest_addr = {0};
		dest_addr.sin_family = AF_INET;
		dest_addr.sin_port = htons(port_);
		dest_addr.sin_addr.s_addr = inet_addr(unicast_address);
		int res = sendto(sock_fd_, buf, len, 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
		return res;
	}
	virtual int recv_unicast(char *recv_buf) override {
		char buf[BUFSIZ] = "";
		socklen_t size = sizeof(struct sockaddr);
		int len = recvfrom(sock_fd_, buf, sizeof(buf), 0, (struct sockaddr *)&client_addr_, &size);
		cout << "connection port = " << client_addr_.sin_port << endl;
		cout << "connection ip = " << inet_ntoa(client_addr_.sin_addr) << endl;
		strncpy(recv_buf, buf, len);
		return len;
	}
	int recv_unicast_return_ack(char *recv_buf, const char *ack) {
		char buf[BUFSIZ] = "";
		socklen_t size = sizeof(struct sockaddr);
		int len = recvfrom(sock_fd_, buf, sizeof(buf), 0, (struct sockaddr *)&client_addr_, &size);
		cout << "connection port = " << client_addr_.sin_port << endl;
		cout << "connection ip = " << inet_ntoa(client_addr_.sin_addr) << endl;
		strncpy(recv_buf, buf, len);
		sendto(sock_fd_, ack, strlen(ack), 0, (struct sockaddr *)&client_addr_, sizeof(client_addr_));
		return len;
	}
	int send_back(const char *buf, int len) {
		return sendto(sock_fd_, buf, len, 0, (struct sockaddr *)&client_addr_, sizeof(client_addr_));
	}
	virtual int send_broadcast(const char *buf, int len, const char *broadcast_address) override {
		return 0;
	}
	virtual int recv_broadcast(char *recv_buf) override {
		return 0;
	}
private:
	int sock_fd_;
	int port_;
	struct sockaddr_in client_addr_;
};

#endif /* UDP_UNICAST_AGENT_HPP_ */
