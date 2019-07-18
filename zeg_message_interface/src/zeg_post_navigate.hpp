/*****************************************************************************
*  zeg post navigate                                                         *
*  Copyright (C) 2019                                                        *
*                                                                            *
*  @file     zeg_post_navigate.hpp                                           *
*  @brief    zeg post navigate command                                       *
*  Details.                                                                  *
*                                                                            *
*  @author                                                                   *
*  @email                                                                    *
*  @version  1.0.0                                                           *
*  @date     2019-06-11                                                      *
*  @license                                                                  *
*                                                                            *
*----------------------------------------------------------------------------*
*  Change History :                                                          *
*  <Date>     | <Version> | <Author>       | <Description>                   *
*----------------------------------------------------------------------------*
*  2019/05/30 | 1.0.0     |                | Create file                     *
* ---------------------------------------------------------------------------*
*  2019/06/06 | 1.0.0     |                | Add call rest rpc               *
*----------------------------------------------------------------------------*
*  2019/06/10 | 1.0.0     |                | Add call simulator              *
*----------------------------------------------------------------------------*
*  2019/06/11 | 1.0.0     |                | Add enqueue for simulator       *
*****************************************************************************/
#ifndef SRC_ZEG_POST_NAVIGATE_HPP_
#define SRC_ZEG_POST_NAVIGATE_HPP_
#include <exception>
#include "base_thread.hpp"
#include "zeg_config.hpp"
#include "rpc_client.hpp"
#include "codec.h"
#include "zeg_robot_define.hpp"
namespace zeg_message_interface {
using namespace rest_rpc;
using namespace rest_rpc::rpc_service;
using namespace zeg_robot_simulator;
class zeg_post_navigate : public base_thread {
public:
	bool init() {
		return init_connect();
	}
	inline bool init_connect() {
		return client_.connect(RPC_SERVER_IP, RPC_SERVER_PORT, 5);
	}
protected:
    virtual void process() override {
        string cmd_str;
        while (true) {
            zeg_config::get_instance().navigate_cmd_queue.wait_dequeue(cmd_str);
            zeg_robot_navigate_command cmd = {0};
            if (false == unpack_command(cmd_str, cmd)) {
                continue;
            }
            LOG_INFO << "get taskid from scheduler server = " << cmd.task_id;
            LOG_INFO << "get pose count from scheduler server = " << cmd.points_.size();
            for (auto &pose : cmd.points_) {
                LOG_INFO << "(" << pose.x << "," << pose.y << ")";
            }
            enqueue_simulator_pose_trace(cmd);
        }
    }
public:
	bool unpack_command(const string &cmd_str, zeg_robot_navigate_command &cmd) {
		msgpack::unpacked msg;
		msgpack::unpack(&msg, cmd_str.c_str(), cmd_str.size());
		msgpack::object obj = msg.get();
		try {
			obj.convert(&cmd);
		}
		catch (std::exception &e) {
			LOG_CRIT << e.what();
			return false;
		}
		LOG_INFO << "navigate command taskid = " << cmd.task_id;
		return true;
	}
	bool call_simulator_get_pose_trace(const zeg_robot_navigate_command &cmd, vector<robot_pose>&pose_trace) {
		bool no_exception = true;
		vector<robot_pose>pose_set;
		for (auto &e : cmd.points_) {
			robot_pose pose(e.x, e.y, 0);
			pose_set.emplace_back(pose);
		}
	    try {
	    	pose_trace = client_.call<vector<robot_pose>>("get_pose_trace", pose_set);
	    }
	    catch (const std::exception& e) {
	    	LOG_CRIT << e.what();
	    	no_exception = false;
	    }
		return no_exception && (false == pose_trace.empty());
	}
	bool call_simulator_get_pose_trace(const zeg_robot_point &point, vector<robot_pose>&pose_trace) {
		bool no_exception = true;
		robot_pose pose(point.x, point.y, 0);
	    try {
	    	pose_trace = client_.call<vector<robot_pose>>("get_pose_trace1", pose);
	    }
	    catch (const std::exception& e) {
	    	LOG_CRIT << e.what();
	    	no_exception = false;
	    }
		return no_exception && (false == pose_trace.empty());
	}
	void enqueue_simulator_pose_trace(const zeg_robot_navigate_command &cmd) {
		vector<robot_pose>pose_trace;
		if (false == call_simulator_get_pose_trace(cmd, pose_trace)) {
			LOG_CRIT << "call_simulator_ get_pose_trace failed...!";
			return;
		}
		for (auto &pose : pose_trace) {
			if (zeg_config::get_instance().simulator_pose_queue.size_approx() > 1000000) {
				LOG_CRIT << "simulator_pose_queue is full...!";
				continue;
			}
			zeg_config::get_instance().simulator_pose_queue.enqueue(pose);
		}
	}
	void enqueue_simulator_pose_trace(const zeg_robot_navigate_command &cmd, int index) {
		vector<robot_pose>pose_trace;
		if (false == call_simulator_get_pose_trace(cmd.points_[index], pose_trace)) {
			LOG_CRIT << "call_simulator_ get_pose_trace failed...!";
			return;
		}
		for (auto &pose : pose_trace) {
			if (zeg_config::get_instance().simulator_pose_queue.size_approx() > 1000000) {
				LOG_CRIT << "simulator_pose_queue is full...!";
				continue;
			}
			zeg_config::get_instance().simulator_pose_queue.enqueue(pose);
		}
	}
private:
	bool unpack_command(const string &cmd_str, znavigate_command &cmd) {
		msgpack::unpacked msg;
		msgpack::unpack(&msg, cmd_str.c_str(), cmd_str.size());
		msgpack::object obj = msg.get();
		try {
			obj.convert(&cmd);
		}
		catch (std::exception &e) {
			LOG_CRIT << e.what();
			return false;
		}
		LOG_INFO << "navigate command taskid = " << cmd.task_id;
		return true;
	}
public:
	rpc_client client_;
public:
	uint64_t test_unpack_command() {
		string cmd_str;
		if (false == zeg_config::get_instance().navigate_cmd_queue.try_dequeue(cmd_str)) {
			return 0;
		}
		znavigate_command cmd = {0};
		if (false == unpack_command(cmd_str, cmd)) {
			return 0;
		}
		return cmd.task_id;
	}
	auto test_get_taskid(uint64_t taskid) {
		znavigate_command cmd = {0};
		cmd.task_id = taskid;
		auto result = client_.call<uint64_t>("get_taskid", cmd);
		return result;
	}
};
}

#endif /* SRC_ZEG_POST_NAVIGATE_HPP_ */
