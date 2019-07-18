#ifndef SRC_ZEG_NAVIGATION_ESCORT_TASK_HPP_
#define SRC_ZEG_NAVIGATION_ESCORT_TASK_HPP_
#include <algorithm>
#include "zeg_robot_define.hpp"
namespace zeg_robot_navigation_escort {
struct zeg_navigation_escort_task {
	inline void load_unsure_lock_points(const vector<zeg_robot_point>&points) {
		unsure_lock_points_.clear();
		sure_status_.clear();
		int size = points.size();
		unsure_lock_points_.resize(size);
		sure_status_.resize(size);
		std::copy(begin(points), end(points), begin(unsure_lock_points_));
	}
	string cur_task_id_;
	string robot_id_;
	vector<zeg_robot_point>unsure_lock_points_;
	vector<bool>sure_status_;	// false - unsure true - sure
};
}

#endif /* SRC_ZEG_NAVIGATION_ESCORT_TASK_HPP_ */
