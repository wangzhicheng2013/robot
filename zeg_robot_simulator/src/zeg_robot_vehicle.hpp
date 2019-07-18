#ifndef SRC_ZEG_ROBOT_VEHICLE_HPP_
#define SRC_ZEG_ROBOT_VEHICLE_HPP_
#include "zeg_data_define.h"
#include "zeg_config.hpp"
#include "zeg_pose_compute.hpp"
namespace zeg_robot_simulator {
class zeg_robot_vehicle  {
public:
	zeg_robot_vehicle() :
		msecs_(zeg_config::get_instance().msecs),
		vehicle_cur_pose_(zeg_config::get_instance().cur_pose_),
		vehicle_speed_(zeg_config::get_instance().speed_) {
		vechicle_id_ = 0;
		vechicle_battery_remaining = 0.88;
		vechicle_status_ = 	ROBOT_VEHICLE_STOP;
		vehicle_speed_.w = deg2rad(vehicle_speed_.w);
	}
	inline void move() {
		 robot_pose motion;
		 motion.x = vehicle_speed_.vx * msecs_ / 1000.0;
		 get_next_pose(motion);
	}
	inline void rotate() {
		 robot_pose motion;
		 motion.theta = vehicle_speed_.w * msecs_ / 1000.0;
		 get_next_pose(motion);
	}
private:
	inline void get_next_pose(const robot_pose &motion) {
		robot_pose next_pose;
		absolute_sum(vehicle_cur_pose_, motion, next_pose);
		LOG_INFO << "robot vehicle " << vechicle_id_ << " move from " << "(" << vehicle_cur_pose_.x << "," << vehicle_cur_pose_.y << "," << vehicle_cur_pose_.theta \
				<< ") to (" << next_pose.x << "," << next_pose.y << "," << next_pose.theta << ")";
		vehicle_cur_pose_ = next_pose;
	}
public:
	int msecs_;
	int vechicle_id_;
	unsigned char vechicle_status_;
	double vechicle_battery_remaining;
	robot_pose vehicle_cur_pose_;
	robot_speed vehicle_speed_;
};

}

#endif /* SRC_ZEG_ROBOT_VEHICLE_HPP_ */
