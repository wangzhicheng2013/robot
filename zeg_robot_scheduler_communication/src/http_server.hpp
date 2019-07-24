#ifndef SRC_HTTP_SERVER_HPP_
#define SRC_HTTP_SERVER_HPP_
#include <vector>
#include <thread>
#include "http_thread.hpp"
class http_server {
public:
	http_server() = default;
	~http_server() {
		for (auto &it : threads_) {
			if (it.joinable()) {
				it.join();
			}
		}
	}
	bool init(const socket_config &config, int network_size = thread::hardware_concurrency()) {
		if (network_size <= 0 || network_size > NETWORKCAPACITY) {
			return false;
		}
		int sock_fd = sockect_utility::get_instance().bind_socket(config);
		if (sock_fd < 0) {
			return false;
		}
		http_threads_.resize(network_size);
		for (auto &it : http_threads_) {
			if (!it.init(sock_fd)) {
				return false;
			}
		}
		threads_.resize(network_size);
		for (int i = 0;i < network_size;i++) {
			try {
				threads_[i] = thread(bind(&http_server::start_http_thread, this, i));
			}
			catch (...) {
				return false;
			}
		}
		return true;
	}
private:
	inline void start_http_thread(int index) {
		http_threads_[index].http_dispath_event();
	}
private:
	static const int NETWORKCAPACITY = 128;
private:
	vector<http_thread>http_threads_;
	vector<thread>threads_;
};




#endif /* SRC_HTTP_SERVER_HPP_ */
