/*****************************************************************************
*  zeg recv navigate class                                                   *
*  Copyright (C) 2019                                                        *
*                                                                            *
*  @file     zeg recv_navigate.hpp                                           *
*  @brief    recv navigate message from client                               *
*  Details.                                                                  *
*                                                                            *
*  @author                                                                   *
*  @email                                                                    *
*  @version  1.0.0                                                           *
*  @date     2019-05-29                                                      *
*  @license                                                                  *
*                                                                            *
*----------------------------------------------------------------------------*
*  Change History :                                                          *
*  <Date>     | <Version> | <Author>       | <Description>                   *
*----------------------------------------------------------------------------*
*  2019/05/29 | 1.0.0     |                | Create file                     *
*----------------------------------------------------------------------------*                                                                   *
*****************************************************************************/
#ifndef SRC_ZEG_RECV_NAVIGATE_HPP_
#define SRC_ZEG_RECV_NAVIGATE_HPP_
#include "zeg_config.hpp"
#include "zmq_agent.hpp"
#include "base_thread.hpp"
namespace zeg_message_interface {
using namespace zmq_self_agent;
class zeg_recv_navigate : public base_thread {
public:
	bool init() {
		config.sock_type = ZMQ_PULL;
		config.addr = zeg_config::get_instance().local_navigate_address;
		unsigned char ret = zmq_server_navigate.init(config);
		if (NO_ERROR !=  ret) {
			LOG_CRIT << "zmq error code = " <<  ret;
			return false;
		}
		this_thread::sleep_for(chrono::seconds(1));
		return true;
	}
protected:
	virtual void process() override {
		string recv_str;
		while (true) {
			zmq_server_navigate.recv(recv_str);
			LOG_INFO << "recv navigate command.";
			if (zeg_config::get_instance().navigate_cmd_queue.size_approx() > zeg_config::get_instance().max_queue_size) {
				LOG_INFO << "queue is full." << "\n";
				continue;
			}
			if (true == zeg_config::get_instance().navigate_cmd_queue.enqueue(recv_str)) {
				zeg_config::get_instance().recv_navigate_cmd_counter_.fetch_add(1, memory_order_release);
			}
		}
	}
public:
	zmq_agent zmq_server_navigate;
	zmq_config config;
};
}

#endif /* SRC_ZEG_RECV_NAVIGATE_HPP_ */
