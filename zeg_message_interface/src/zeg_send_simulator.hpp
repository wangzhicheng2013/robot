/*****************************************************************************
*  zeg send simulator data                                                   *
*  Copyright (C) 2019                                                        *
*                                                                            *
*  @file     zeg_send_simulator.hpp                                          *
*  @brief    send simulator message to scheduler server                      *
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
*  2019/05/29 | 1.0.0     |                | Create file                     *
*----------------------------------------------------------------------------*                                                                   *
*****************************************************************************/
#ifndef SRC_ZEG_SEND_SIMULATOR_HPP_
#define SRC_ZEG_SEND_SIMULATOR_HPP_
#include "zeg_config.hpp"
#include "zmq_agent.hpp"
#include "base_thread.hpp"
namespace zeg_message_interface {
using namespace zmq_self_agent;
class zeg_send_simulator : public base_thread {
public:
	bool init() {
		config.sock_type = ZMQ_PUSH;
		config.addr = zeg_config::get_instance().pose_report_address.c_str();
		unsigned char ret = zmq_client_simulator.init(config);
		if (NO_ERROR !=  ret) {
			LOG_CRIT << "zmq error code = " <<  ret;
			return false;
		}
		this_thread::sleep_for(chrono::seconds(1));
		return true;
	}
	void push_pose(const robot_pose &pose) {
		pack_robot_pose(pose);
		zmq_client_simulator.send(buffer.data(), buffer.size());
	}
protected:
	virtual void process() override {
		robot_pose cur_pose;
		robot_pose pose;
		while (true) {
			if (true == zeg_config::get_instance().simulator_pose_queue.try_dequeue(pose)) {
				cur_pose = pose;
			}
			pack_robot_pose(pose);
			zmq_client_simulator.send(buffer.data(), buffer.size());
			this_thread::sleep_for(chrono::milliseconds(zeg_config::get_instance().robot_simulator_msecs));
		}
	}
public:
	inline void pack_robot_pose(const robot_pose &pose) {
		buffer.clear();
		msgpack::pack(buffer, pose);
	}
public:
	zmq_agent zmq_client_simulator;
	zmq_config config;
	msgpack::sbuffer buffer;
};
}

#endif /* SRC_ZEG_SEND_SIMULATOR_HPP_ */
