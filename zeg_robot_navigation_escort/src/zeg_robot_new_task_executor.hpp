#ifndef SRC_ZEG_ROBOT_NEW_TASK_EXECUTOR_HPP_
#define SRC_ZEG_ROBOT_NEW_TASK_EXECUTOR_HPP_
#include "base_thread.hpp"
#include "rpc_client.hpp"
#include "codec.h"
#include "common_utility.hpp"
#include "zeg_robot_new_task_loader.hpp"
namespace zeg_robot_navigation_escort {
using namespace rest_rpc;
using namespace rest_rpc::rpc_service;
enum {
	ONE_POINT_FOR_ONE_LOCK_POINT_POLICY,
	MUTIPLE_POINT_FOR_ONE_LOCK_POINT_POLICY
};
class zeg_robot_new_task_executor : public base_thread {
public:
	zeg_robot_new_task_executor() : rpc_client_ptr_(new rpc_client) {
		rpc_connected_ = false;
		policy_ = ONE_POINT_FOR_ONE_LOCK_POINT_POLICY;
	}
	bool init() {
		if (nullptr == rpc_client_ptr_) {
			return false;
		}
		rpc_connected_ = rpc_client_ptr_->connect(zeg_config::zeg_config::get_instance().RPC_SERVER_IP, zeg_config::get_instance().robot_rpc_message_interface_layer_port, 5);
		return true;
	}
	inline void set_policy(unsigned char policy) {
		policy_ = policy;
	}
	inline void make_lock_point_with_one_for_one_policy(const zeg_robot_point &point, zeg_robot_navigation_lock_point &lock_point) {
		make_uuid(lock_point.task_id);
		lock_point.locked_points.clear();
		lock_point.locked_points.emplace_back(point);
	}
	bool call_send_navigation_lock_point(const zeg_navigation_escort_task &escort_task,
			const zeg_robot_navigation_lock_point &lock_point) {
        bool no_exception = true;
        bool res = true;
        if (false == rpc_connected_) {
            rpc_connected_ = rpc_client_ptr_->connect(zeg_config::get_instance().RPC_SERVER_IP, zeg_config::get_instance().robot_rpc_message_interface_layer_port, 5);
        }
		zeg_robot_header header("zeg.robot.navigation.lock.point", escort_task.robot_id_, chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count());
        try {
        	res = rpc_client_ptr_->call<bool>("send_navigation_lock_point", header, lock_point);
        }
        catch (const std::exception& e) {
            LOG_CRIT << e.what();
            no_exception = false;
        }
        if (false == res) {
        	LOG_CRIT << "rpc call_send_navigation_lock_point failed.";
        }
        return no_exception && (true == res);
	}
protected:
	virtual void process() override {
		while (true) {
			auto &mutex_ = zeg_robot_new_task_loader::get_instance().mutex_;
			auto &cond_ = zeg_robot_new_task_loader::get_instance().cond_;
			auto &new_task_ = zeg_robot_new_task_loader::get_instance().new_task_;
			auto &cur_task_ = zeg_robot_new_task_loader::get_instance().cur_task_;
			unique_lock<mutex>lk(mutex_);
			cond_.wait(lk, [&new_task_] {return true == new_task_;});
			new_task_ = false;
			lk.unlock();
			zeg_robot_navigation_lock_point lock_point;
			for (auto &point : cur_task_.unsure_lock_points_) {
				lock_guard<mutex>lk(mutex_);
				if (true == new_task_) {
					break;
				}
				make_lock_point_with_one_for_one_policy(point, lock_point);
				call_send_navigation_lock_point(cur_task_ , lock_point);
			}
		}
	}
private:
	unique_ptr<rpc_client>rpc_client_ptr_;
private:
	bool rpc_connected_;
	unsigned char policy_;
};
}



#endif /* SRC_ZEG_ROBOT_NEW_TASK_EXECUTOR_HPP_ */
