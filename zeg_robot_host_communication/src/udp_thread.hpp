#ifndef SRC_UDP_THREAD_HPP_
#define SRC_UDP_THREAD_HPP_
#include "socket_utility.hpp"
#include "event.h"
#include "event2/listener.h"
class udp_thread {
public:
	udp_thread() {
		sock_fd_ = -1;
		base_ = nullptr;
		event_ = nullptr;
	}
	virtual ~udp_thread() {
		if (event_ != nullptr) {
			event_free(event_);
			event_ = nullptr;
		}
		if (base_ != nullptr) {
			event_base_loopexit(base_, NULL);
			event_base_free(base_);
			base_ = nullptr;
		}
	}
public:
	bool init(int fd) {
		if (fd < 0) {
			return false;
		}
		sock_fd_ = fd;
		base_ = event_base_new();
		if (nullptr == base_) {
			return false;
		}
		event_ = event_new(base_, sock_fd_, EV_READ | EV_PERSIST, read_udp_cb, this);
		if (nullptr == event_) {
			return false;
		}
		return 0 == event_add(event_, NULL);
	}
	void udp_dispatch_event() {
		if (base_ != nullptr) {
			event_base_dispatch(base_);
		}
	}
	virtual bool deal_udp_message(int fd) = 0;
public:
	static void read_udp_cb(int fd, short event, void *arg) {
		udp_thread *p = reinterpret_cast<udp_thread *>(arg);
		if (nullptr == p) {
			return;
		}
		p->deal_udp_message(fd);
	}
protected:
	int sock_fd_;
	event_base *base_;
	event *event_;
public:
	static const int buffer_size = 1024;
};


#endif /* SRC_UDP_THREAD_HPP_ */
