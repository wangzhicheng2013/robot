#ifndef SRC_ZEG_POSE_COMPUTE_HPP_
#define SRC_ZEG_POSE_COMPUTE_HPP_
#include <thread>
#include <chrono>
#include "zeg_robot_define.hpp"
namespace zeg_robot_simulator {
class pose_compute {
public:
	pose_compute() {
		msecs = 100;
		theta = 0;
		speed_.w = deg2rad(speed_.w);
	}
	pose_compute(int msecs,
			const robot_pose &cur_pose,
			const robot_pose &destination_pose,
			const robot_speed &speed) : msecs(msecs),
					cur_pose_(cur_pose), destination_pose_(destination_pose), speed_(speed) {
		theta = 0;
		speed_.w = deg2rad(speed_.w);
	}
	pose_compute(const pose_compute &) = delete;
	pose_compute & operator = (const pose_compute &) = delete;
	virtual ~pose_compute() = default;
public:
	void get_next_pose(robot_pose &next_pose) {
		 robot_pose motion;
		 double v = speed_.vx;
		 double w = speed_.w;
		 if (!equal(speed_.w, 0.0) && !equal(v, 0.0)) {
			 double r = fabs(v / w);
			 double theta = fabs(w * msecs / 1000.0);
			 motion.x = sin(theta) * r * sign(v);
			 motion.y = ((1 - cos(theta)) * r) * sign(v) * sign(w);
			 motion.theta = theta * sign(w);
		 }
		 else if (equal(speed_.w, 0.0)) {
			 motion.x = v * msecs / 1000.0;
		 }
		 else {
			 motion.theta = w * msecs / 1000.0;
		 }
		 absolute_sum(cur_pose_, motion, next_pose);
	}
	bool get_pose_trace(vector<robot_pose>&pose_trace) {
		pose_trace.clear();
		double s = speed_.vx * msecs / 1000;
		double d = distance(cur_pose_, destination_pose_);
		int loop = 2 * ceil(d / (msecs / 1000.0));
		int count = 0;
		robot_pose next_pose;
		while (d > s) {
			get_next_pose(next_pose);
			pose_trace.emplace_back(next_pose);
			cur_pose_ = next_pose;
			d = distance(cur_pose_, destination_pose_);
			if (++count > loop) {
				return false;
			}
		}
		return true;
	}
	bool adjust_pose_angle() {
		if (0 == speed_.w) {
			return false;
		}
		theta = direction(cur_pose_, destination_pose_);
		double normalized_value = normalize(theta - cur_pose_.theta);
		if (fabs(normalized_value) <= speed_.w * msecs / 1000) {
			cur_pose_.theta = theta;
			return false;
		}
		return true;
	}
	inline void rotate_robot_adjust() {
		destination_pose_.theta = theta;
		speed_.vx = 0;
	}
	void rotate_robot_pose(vector<robot_pose>&pose_trace) {
		pose_trace.clear();
		if (false == adjust_pose_angle()) {
			return;
		}
		double tmp = theta;
		double destination_theta = destination_pose_.theta;
		double speed_vx = speed_.vx;
		rotate_robot_adjust();
		robot_pose next_pose;
		do {
			get_next_pose(next_pose);
			pose_trace.emplace_back(next_pose);
			cur_pose_ = next_pose;
		}while (true == adjust_pose_angle());
		destination_pose_.theta = destination_theta;
		speed_.vx = speed_vx;
		cur_pose_.theta = tmp;
	}
	bool get_pose_trace_with_angle(const vector<robot_pose>&pose_set, vector<robot_pose>&pose_trace) {
		vector<robot_pose>pose_tmp;
		pose_trace.clear();
		for (auto &pose : pose_set) {
			destination_pose_ = pose;
			rotate_robot_pose(pose_tmp);
			merge_vector(pose_tmp, pose_trace);
			double tmp = speed_.w;
			speed_.w = 0;
			if (false == get_pose_trace(pose_tmp)) {
				return false;
			}
			speed_.w = tmp;
			merge_vector(pose_tmp, pose_trace);
			pose_trace.emplace_back(destination_pose_);
		}
		return true;
	}
	bool get_pose_trace_with_angle(const robot_pose &target_pose, vector<robot_pose>&pose_trace) {
		vector<robot_pose>pose_tmp;
		pose_trace.clear();
		destination_pose_= target_pose;
		rotate_robot_pose(pose_tmp);
		merge_vector(pose_tmp, pose_trace);
		double tmp = speed_.w;
		speed_.w = 0;
		if (false == get_pose_trace(pose_tmp)) {
			return false;
		}
		speed_.w = tmp;
		merge_vector(pose_tmp, pose_trace);
		pose_trace.emplace_back(destination_pose_);
		return true;
	}
private:
	void merge_vector(vector<robot_pose>&v0, vector<robot_pose>&v1) {
		for_each(begin(v0), end(v0), [&v1](auto &e) {
			e.theta = rad2deg(e.theta);
			v1.emplace_back(e);
		});
	}
public:
	int msecs;				// ms
	double theta;
	robot_pose cur_pose_;
	robot_pose destination_pose_;
	robot_speed speed_;
};

}

#endif /* SRC_ZEG_POSE_COMPUTE_HPP_ */
