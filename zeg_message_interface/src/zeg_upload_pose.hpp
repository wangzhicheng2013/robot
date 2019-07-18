/*****************************************************************************
*  zeg upload pose to scheduler server                                       *
*  Copyright (C) 2019                                                        *
*                                                                            *
*  @file     zeg_send_simulator.hpp                                          *
*  @brief    upload pose message to scheduler server                         *
*  Details.                                                                  *
*                                                                            *
*  @author                                                                   *
*  @email                                                                    *
*  @version  1.0.0                                                           *
*  @date     2019-06-13                                                      *
*  @license                                                                  *
*                                                                            *
*----------------------------------------------------------------------------*
*  Change History :                                                          *
*  <Date>     | <Version> | <Author>       | <Description>                   *
*----------------------------------------------------------------------------*
*  2019/06/13 | 1.0.0     |                | Create file                     *
*----------------------------------------------------------------------------*                                                                   *
*****************************************************************************/
#ifndef SRC_ZEG_UPLOAD_POSE_HPP_
#define SRC_ZEG_UPLOAD_POSE_HPP_
#include "zeg_config.hpp"
#include "zmq_agent.hpp"
#include "rpc_server.h"
namespace zeg_message_interface {
using namespace zmq_self_agent;
using namespace rest_rpc;
using namespace rpc_service;
class zeg_upload_pose {
public:
	bool init() {
		config_.sock_type = ZMQ_PUSH;
		config_.addr = zeg_config::get_instance().pose_upload_address.c_str();
		unsigned char ret = zmq_client_.init(config_);
		if (NO_ERROR !=  ret) {
			LOG_CRIT << "zmq error code = " <<  ret;
			return false;
		}
		this_thread::sleep_for(chrono::seconds(1));
		return true;
	}
	bool upload_pose(rpc_conn conn, const robot_pose &pose) {
		pack_robot_pose(pose);
		return buffer_.size() == zmq_client_.send(buffer_.data(), buffer_.size());
	}
private:
	inline void pack_robot_pose(const robot_pose &pose) {
		buffer_.clear();
		msgpack::pack(buffer_, pose);
	}
private:
	zmq_agent zmq_client_;
	zmq_config config_;
	msgpack::sbuffer buffer_;
};
}

#endif /* SRC_ZEG_UPLOAD_POSE_HPP_ */
