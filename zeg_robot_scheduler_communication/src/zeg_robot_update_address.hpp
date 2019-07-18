#ifndef SRC_ZEG_ROBOT_UPDATE_ADDRESS_HPP_
#define SRC_ZEG_ROBOT_UPDATE_ADDRESS_HPP_
#include <netinet/in.h>
#include <unordered_map>
#include <mutex>
#include <memory>
#include "NanoLog.hpp"
namespace zeg_robot_scheduler_communication {
using namespace std;
class zeg_robot_update_address {
public:
	inline static zeg_robot_update_address &get() {
		static zeg_robot_update_address obj;
		return obj;
	}
	void update(const string &robot_id, const struct sockaddr_in &client_addr) {
		lock_guard<mutex>lk(lock_);
		auto it = map_.find(robot_id);
		if (end(map_) == it) {
			map_[robot_id] = client_addr;
			LOG_INFO << "robot id = " << robot_id << " robot ip = " << inet_ntoa(client_addr.sin_addr) << ":" << client_addr.sin_port;
			return;
		}
		if (client_addr.sin_port != it->second.sin_port) {
			it->second.sin_port = client_addr.sin_port;
			LOG_INFO << "robot id = " << robot_id << " updated robot ip = " << inet_ntoa(client_addr.sin_addr) << ":" << client_addr.sin_port;
		}
		if (client_addr.sin_addr.s_addr != it->second.sin_addr.s_addr) {
			it->second.sin_addr.s_addr = client_addr.sin_addr.s_addr;
			LOG_INFO << "robot id = " << robot_id << " updated robot ip = " << inet_ntoa(client_addr.sin_addr) << ":" << client_addr.sin_port;
		}
	}
	const struct sockaddr_in *get(const string &robot_id) {
		auto it = map_.find(robot_id);
		if (end(map_) == it) {
			return nullptr;
		}
		return &(it->second);
	}
	inline int get_client_num() {
		return map_.size();
	}
private:
	zeg_robot_update_address() = default;
	~zeg_robot_update_address() = default;
private:
	unordered_map<string, struct sockaddr_in>map_;
	mutex lock_;
};
}

#endif /* SRC_ZEG_ROBOT_UPDATE_ADDRESS_HPP_ */
