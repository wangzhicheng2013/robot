#ifndef SRC_ZEG_ROBOT_TASK_ESCORT_HPP_
#define SRC_ZEG_ROBOT_TASK_ESCORT_HPP_
#include <vector>
#include <unordered_map>
#include <mutex>
#include <thread>
#include "zeg_robot_command_pack.hpp"
#include "zeg_robot_update_address.hpp"
#include "blockingconcurrentqueue.h"
#include "zeg_robot_config.hpp"
namespace zeg_robot_scheduler_communication {
using namespace moodycamel;
class zeg_robot_task_escort {
public:
	inline static zeg_robot_task_escort &get() {
		static zeg_robot_task_escort obj;
		return obj;
	}
	inline void remove_robot_task(const string &task_id) {
		lock_guard<mutex>lk(lock_);
		map_.erase(task_id);
	}
	inline bool get_task_pack_str(const string &task_id, string &pack_str) {
		auto it = map_.find(task_id);
		if (end(map_) == it) {
			return false;
		}
		pack_str = it->second;
		return true;
	}
	void store_robot_task(const string &task_id, const string &pack_str) {
		unique_lock<mutex>lk(lock_);
		auto it = map_.find(task_id);
		if (end(map_) != it) {
			return;
		}
		if (map_.size() > zeg_robot_config::get_instance().robot_task_escort_message_backlog) {
			LOG_CRIT << "task count for map is up to max.";
			return;
		}
		map_[task_id] = pack_str;
		lk.unlock();
		if (queue_.size_approx() > zeg_robot_config::get_instance().robot_task_escort_message_backlog) {
			LOG_CRIT << "task count for queue is up to max.";
			return;
		}
		queue_.enqueue(task_id);
		LOG_INFO << "task id = " << task_id << " robot task store ok.";
	}
	void get_taskid(string &taskid) {
		queue_.wait_dequeue(taskid);
	}
private:
	zeg_robot_task_escort() = default;
	~zeg_robot_task_escort() = default;
private:
	unordered_map<string, string>map_;			// key -- task id value -- pack string
	mutex lock_;
	BlockingConcurrentQueue<string>queue_;		// task id
};
}

#endif /* SRC_ZEG_ROBOT_TASK_ESCORT_HPP_ */
