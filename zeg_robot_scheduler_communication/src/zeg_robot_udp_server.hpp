#ifndef SRC_ZEG_ROBOT_UDP_SERVER_HPP_
#define SRC_ZEG_ROBOT_UDP_SERVER_HPP_
#include <vector>
#include <thread>
#include <memory>
#include "message_communicate_entity_maker.hpp"
#include "zeg_robot_udp_thread.hpp"
namespace zeg_robot_scheduler_communication {
class zeg_robot_udp_server : public message_communicate_entity {
public:
	zeg_robot_udp_server() {
		sock_fd_ = -1;
		memset(&broadcast_addr_, 0, sizeof(broadcast_addr_));
		make_broadcast_addr();
	}
	virtual ~zeg_robot_udp_server() {
		for (auto &it : threads_) {
			if (it.joinable()) {
				it.join();
			}
		}
		sockect_utility::get_instance().close_socket(sock_fd_);
	}
public:
	inline bool init() override {
		return init(thread::hardware_concurrency());
	}
	void set_config(const socket_config &config) {
		config_ = config;
	}
	bool init(int network_size) {
		if (network_size <= 0 || network_size > network_capacity) {
			return false;
		}
		sock_fd_ = sockect_utility::get_instance().bind_socket(config_);
		if (sock_fd_ < 0) {
			return false;
		}
		threads_.resize(network_size);
		for (int i = 0;i < network_size;i++) {
			udp_threads_.emplace_back(unique_ptr<udp_thread>(new zeg_robot_udp_thread));
			if (false == udp_threads_[i]->init(sock_fd_)) {
				cerr << "udp thread init failed...!" << endl;
				continue;
			}
			try {
				threads_[i] = thread(std::bind(&zeg_robot_udp_server::start_udp_thread, this, i));
			}
			catch (std::exception &e) {
				LOG_CRIT << e.what();
				return false;
			}
		}
		return true;
	}
	virtual int send(const char *buf, int len) override {
		return -1;
	}
	int send_to_client(const char *buf, int len, const struct sockaddr_in &client_addr) {
		return sendto(sock_fd_, buf, len, 0, (struct sockaddr *)&client_addr, sizeof(client_addr));
	}
	virtual int recv(char *recv_buf, int buf_len) override {
		return -1;
	}
	int send_broadcast(const char *buf, int len) {
		return sendto(sock_fd_, buf, len, 0, (struct sockaddr *)&broadcast_addr_, sizeof(broadcast_addr_));
	}
private:
	void start_udp_thread(int index) {
		udp_threads_[index]->udp_dispatch_event();
	}
	inline void make_broadcast_addr() {
		broadcast_addr_.sin_family = AF_INET;
		broadcast_addr_.sin_port = htons(config_.port_);
		broadcast_addr_.sin_addr.s_addr = inet_addr("255.255.255.255");
	}
private:
	struct sockaddr_in broadcast_addr_;
	socket_config config_;
	vector<unique_ptr<udp_thread> >udp_threads_;
	vector<thread>threads_;
private:
	int sock_fd_;
private:
	const int network_capacity = 128;
};
REGISTER_MESSAGE_COMMUNICATE_ENTITY(zeg_robot_udp_server, "zeg.robot.udp.server");
}
#endif /* SRC_ZEG_ROBOT_UDP_SERVER_HPP_ */
