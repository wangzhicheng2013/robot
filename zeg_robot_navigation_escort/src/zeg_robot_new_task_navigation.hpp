#ifndef SRC_ZEG_ROBOT_NEW_TASK_NAVIGATION_HPP_
#define SRC_ZEG_ROBOT_NEW_TASK_NAVIGATION_HPP_
#include "zeg_robot_new_task_loader.hpp"
#include "rpc_client.hpp"
#include "codec.h"
namespace zeg_robot_navigation_escort {
using namespace rest_rpc;
using namespace rest_rpc::rpc_service;
class zeg_robot_new_task_navigation {
public:
	inline static zeg_robot_new_task_navigation &get_instance() {
		static zeg_robot_new_task_navigation navigation;
		return navigation;
	}
	void navigate_task(const zeg_robot_header &header, const zeg_robot_navigation_lock_point_ack &cmd) {
		update_unsure_lock_points(cmd);
		call_report_poses_to_robot_host(header, cmd);
	}
	void update_unsure_lock_points(const zeg_robot_navigation_lock_point_ack &cmd) {
		LOG_INFO << cmd.task_id << " lock point ack.";
		auto &mutex_ = zeg_robot_new_task_loader::get_instance().mutex_;
		auto &cur_task_ = zeg_robot_new_task_loader::get_instance().cur_task_;
		int i = 0;
		lock_guard<mutex>lk(mutex_);
		int size = cur_task_ .unsure_lock_points_.size();
		for (auto &point : cmd.locked_points) {
			for (i = 0;i < size;i++) {
				if (point == cur_task_.unsure_lock_points_[i]) {
					cur_task_.sure_status_[i] = true;
					break;
				}
			}
		}
	}
private:
	bool call_report_poses_to_robot_host(const zeg_robot_header &header, const zeg_robot_navigation_lock_point_ack &cmd) {
		static rpc_client client(zeg_config::zeg_config::get_instance().RPC_SERVER_IP, zeg_config::get_instance().robot_rpc_robot_host_layer_port);
		if (false == client.connect(1)) {
			return false;
		}
		vector<zeg_robot_pose>way_points;
		construct_robot_poses(cmd, way_points);
		if (false == client.call<bool>("report_poses_to_simulator", way_points, header.robot_id)) {
			LOG_CRIT << "report_poses_to_simulator rpc call failed...!";
			return false;
		}
		return true;
	}
	void construct_robot_poses(const zeg_robot_navigation_lock_point_ack &cmd, vector<zeg_robot_pose>&way_points) {
		for (auto &point : cmd.locked_points) {
			way_points.emplace_back(zeg_robot_pose(point.x, point.y, 0));
		}
	}
private:
	zeg_robot_new_task_navigation() = default;
	zeg_robot_new_task_navigation(const zeg_robot_new_task_navigation &) = delete;
	zeg_robot_new_task_navigation &operator = (const zeg_robot_new_task_navigation &) = delete;
	virtual ~zeg_robot_new_task_navigation() = default;
};
}

#endif /* SRC_ZEG_ROBOT_NEW_TASK_NAVIGATION_HPP_ */
