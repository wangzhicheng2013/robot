#ifndef SRC_ZEG_ROBOT_POSES_HPP_
#define SRC_ZEG_ROBOT_POSES_HPP_
#include <vector>
#include <queue>
#include <map>
#include <mutex>
#include "zeg_robot_define.hpp"
namespace zeg_robot_simulator {
using namespace std;
class zeg_robot_poses {//todo: rename to waypoints  refactor
public:
	static inline zeg_robot_poses &get_instance() {
		return zeg_robot_poses_obj_;
	}
	bool update_robot_poses(const vector<robot_pose>&poses, int id = 0) {
		deque<robot_pose>d;
		lock_guard<mutex>lk(mutex_);
		auto it = id_poses_.find(id);
		if (it != end(id_poses_)) {
			id_poses_.clear();
		}
		else {
			id_poses_[id] = d;
		}
		for (auto &e : poses) {
			id_poses_[id].emplace_back(e);
		}
		re_navigate = true;
		return id_poses_[id].size() == poses.size();
	}
	bool get_robot_pose(robot_pose &pose, int id = 0) {
		lock_guard<mutex>lk(mutex_);
		auto it = id_poses_.find(id);
		if (it == end(id_poses_) || (0 == it->second.size())) {
			return false;
		}
		pose = id_poses_[id].front();
		id_poses_[id].pop_front();
		re_navigate = false;
		return true;
	}
	bool empty(int id = 0) {
		lock_guard<mutex>lk(mutex_);
		return id_poses_[id].empty();
	}
	inline bool is_re_navigate() {
		lock_guard<mutex>lk(mutex_);
		return re_navigate;
	}
private:
	zeg_robot_poses() {
		re_navigate = false;
	}
	~zeg_robot_poses() = default;
private:
	static zeg_robot_poses zeg_robot_poses_obj_;
public:
	bool re_navigate;
	map<int,  deque<robot_pose>>id_poses_;
	mutex mutex_;
};
zeg_robot_poses zeg_robot_poses::zeg_robot_poses_obj_;
}



#endif /* SRC_ZEG_ROBOT_POSES_HPP_ */
