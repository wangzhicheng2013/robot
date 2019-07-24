#ifndef SRC_HTTP_THREAD_HPP_
#define SRC_HTTP_THREAD_HPP_
#include "socket_utility.hpp"
#include "http_utility.hpp"
class http_thread {
public:
	http_thread() {
		sock_fd = -1;
		event_base_ptr_ = nullptr;
		event_http_ptr_ = nullptr;
	}
	http_thread(const http_thread &other) {
		if (&other != this) {
			sock_fd = other.sock_fd;
			event_base_ptr_ = other.event_base_ptr_;
			event_http_ptr_ = other.event_http_ptr_;
		}
	}
	http_thread & operator = (const http_thread &other) {
		if (&other != this) {
			sock_fd = other.sock_fd;
			event_base_ptr_ = other.event_base_ptr_;
			event_http_ptr_ = other.event_http_ptr_;
		}
		return *this;
	}
	~http_thread() {
		if (nullptr != event_base_ptr_) {
			event_base_loopexit(event_base_ptr_, nullptr);
		}
		if (nullptr != event_http_ptr_)
		{
			evhttp_free(event_http_ptr_);
			event_http_ptr_ = nullptr;
		}
		if (nullptr != event_base_ptr_) {
			event_base_free(event_base_ptr_);
			event_base_ptr_ = nullptr;
		}
	}
	bool init(int fd) {
		sock_fd = fd;
		if (sock_fd < 0) {
			return false;
		}
		event_base_ptr_ = event_base_new();
		if (nullptr == event_base_ptr_) {
			return false;
		}
		event_http_ptr_ = evhttp_new(event_base_ptr_);
		if (nullptr == event_http_ptr_) {
			return false;
		}
		if (evhttp_accept_socket(event_http_ptr_, sock_fd)) {
			return false;
		}
		evhttp_set_gencb(event_http_ptr_, &http_thread::http_call_back_fun, nullptr);
		return true;
	}
	inline void http_dispath_event() {
		if (event_base_ptr_ != nullptr) {
			event_base_dispatch(event_base_ptr_);
		}
	}
private:
	static void http_call_back_fun(struct evhttp_request *req, void *arg) {
		http_utility::get().set_http_utility(req);
		http_utility::get().handle_http();
	}
private:
	int sock_fd;
	event_base *event_base_ptr_;
	evhttp *event_http_ptr_;
};

#endif /* SRC_HTTP_THREAD_HPP_ */
