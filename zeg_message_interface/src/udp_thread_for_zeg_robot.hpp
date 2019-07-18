#ifndef SRC_UDP_THREAD_FOR_ZEG_ROBOT_HPP_
#define SRC_UDP_THREAD_FOR_ZEG_ROBOT_HPP_
#include "udp_thread.hpp"
#include "zeg_robot_command_processor.hpp"
using namespace zeg_message_interface;
class udp_thead_for_zeg_robot : public udp_thread {
public:
	udp_thead_for_zeg_robot() {
		size_ = sizeof(struct sockaddr);
		memset(buf_, 0, sizeof(buf_));
		memset(&client_addr_, 0, sizeof(client_addr_));
	}
	virtual ~udp_thead_for_zeg_robot() = default;
public:
	virtual bool deal_udp_message(int fd) {
		int len = recvfrom(fd, buf_, sizeof(buf_), 0, (struct sockaddr *)&client_addr_, &size_);
		if (false == check_len(len)) {
			return false;
		}
		log_connection_info(len);
		string ack_str;
		if (false == zeg_robot_command_processor::get_instance().process(buf_, len, ack_str)) {
			return false;
		}
		if (false == ack_str.empty()) {
			sendto(fd, ack_str.c_str(), ack_str.size(), 0, (struct sockaddr *)&client_addr_, size_);
		}
		get_schedule_server_addr();
		return true;
	}
private:
	inline bool check_len(int len) {
		if (len < 0) {
			return false;
		}
		if (0 == len) {
			LOG_CRIT << "connection closed...!";
			return false;
		}
		return true;
	}
	inline void log_connection_info(int len) {
		LOG_INFO << "connection ip = " << inet_ntoa(client_addr_.sin_addr) << ":" << client_addr_.sin_port;
		LOG_INFO << "server recv message len = " << len;
	}
	inline void get_schedule_server_addr() {
		if (client_addr_.sin_port != scheduler_client_addr_.sin_port ||
				strcmp(inet_ntoa(client_addr_.sin_addr), inet_ntoa(scheduler_client_addr_.sin_addr))) {
			memcpy(&scheduler_client_addr_, &client_addr_, sizeof(client_addr_));
		}
	}
public:
	static struct sockaddr_in scheduler_client_addr_;
private:
	socklen_t size_;
	char buf_[buffer_size];
	struct sockaddr_in client_addr_;
};
struct sockaddr_in udp_thead_for_zeg_robot::scheduler_client_addr_ = {0};

#endif /* SRC_UDP_THREAD_FOR_ZEG_ROBOT_HPP_ */
