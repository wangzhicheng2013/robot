#ifndef SRC_ZEG_ROBOT_NEW_TASK_LOADER_HPP_
#define SRC_ZEG_ROBOT_NEW_TASK_LOADER_HPP_
#include <atomic>
#include <mutex>
#include <condition_variable>
#include "zeg_config.hpp"
#include "zeg_navigation_escort_task.hpp"
namespace zeg_robot_navigation_escort {
using namespace rest_rpc;
using namespace rest_rpc::rpc_service;
class zeg_robot_new_task_loader {
public:
	inline static zeg_robot_new_task_loader &get_instance() {
		static zeg_robot_new_task_loader loader;
		return loader;
	}
	inline void load(const zeg_robot_header &header, const zeg_robot_navigation_command &cmd) {
		if (true == task_is_repeated(cmd.task_id)) {
			LOG_CRIT << cmd.task_id << " repeated.";
			return;
		}
		update_cur_task(header, cmd);
	}
	inline bool task_is_repeated(const string &task_id) {
		lock_guard<mutex>lk(mutex_);
		return task_id == cur_task_.cur_task_id_;
	}
	inline void update_cur_task(const zeg_robot_header &header, const zeg_robot_navigation_command &cmd) {
		lock_guard<mutex>lk(mutex_);
		cur_task_.cur_task_id_ = cmd.task_id;
		cur_task_.robot_id_ = header.robot_id;
		cur_task_.load_unsure_lock_points(cmd.points);
		new_task_ = true;
		cond_.notify_all();
	}
private:
	zeg_robot_new_task_loader() {
		new_task_ = false;
	}
	zeg_robot_new_task_loader(const zeg_robot_new_task_loader &) = delete;
	zeg_robot_new_task_loader &operator = (const zeg_robot_new_task_loader &) = delete;
	virtual ~zeg_robot_new_task_loader() = default;
public:
	zeg_navigation_escort_task cur_task_;
	bool new_task_;
	mutex mutex_;
	condition_variable cond_;
};
}

#endif /* SRC_ZEG_ROBOT_NEW_TASK_LOADER_HPP_ */
