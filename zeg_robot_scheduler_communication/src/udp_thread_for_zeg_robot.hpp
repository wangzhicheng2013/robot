#ifndef SRC_UDP_THREAD_FOR_ZEG_ROBOT_HPP_
#define SRC_UDP_THREAD_FOR_ZEG_ROBOT_HPP_
#include "udp_thread.hpp"
#include "zeg_robot_command_processor.hpp"
using namespace zeg_robot_scheduler_communication;
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
		zeg_robot_process_arg arg(buf_, len, &client_addr_);
		if (false == zeg_robot_command_processor::get_instance().process(arg)) {
			return false;
		}
		if (false == arg.ack_str.empty()) {
			sendto(fd, arg.ack_str.c_str(), arg.ack_str.size(), 0, (struct sockaddr *)&client_addr_, size_);
		}
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
private:
	socklen_t size_;
	char buf_[buffer_size];
	struct sockaddr_in client_addr_;
};

#endif /* SRC_UDP_THREAD_FOR_ZEG_ROBOT_HPP_ */
