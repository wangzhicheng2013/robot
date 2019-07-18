#ifndef SRC_ZEG_NAVIGATION_LOCK_POINT_ACK_PARSER_HPP_
#define SRC_ZEG_NAVIGATION_LOCK_POINT_ACK_PARSER_HPP_
#include "zeg_robot_command_parser_maker.hpp"
namespace zeg_message_interface {
class zeg_navigation_lock_point_ack_parser : public zeg_command_parser {
public:
public:
	virtual bool parse(zeg_command_unpack_struct *unpack_cmd, string &ack_str) override {
		zeg_robot_navigation_lock_point_ack cmd;
		msgpack::unpack(unpack_cmd->unpack_msg, unpack_cmd->unpack_buf, unpack_cmd->unpack_len, &unpack_cmd->unpack_offset);
		msgpack::object obj = unpack_cmd->unpack_msg->get();
		try {
			obj.convert(&cmd);
		}
		catch(const std::exception &e) {
			LOG_CRIT << e.what();
			return false;
		}
		LOG_INFO << cmd.task_id << " parse navigation lock ack ok.";
	//	call_report_lock_point_to_navigation_escort(unpack_cmd, cmd);
		return true;
	}
private:
	bool call_report_lock_point_to_navigation_escort(const zeg_command_unpack_struct *unpack_cmd,
			const zeg_robot_navigation_lock_point_ack &cmd) {
		static rpc_client client(zeg_config::zeg_config::get_instance().RPC_SERVER_IP, zeg_config::get_instance().robot_rpc_navigation_escort_layer_port);
		if (false == client.connect(1)) {
			LOG_CRIT << "robot navigation escort rpc connect failed...!";
			return false;
		}
		if (false == client.call<bool>("report_lock_point_ack_to_navigation_escort", *(unpack_cmd->unpack_header), cmd)) {
			LOG_CRIT << "report_lock_point_ack_to_navigation_escort rpc call failed...!";
			return false;
		}
		LOG_INFO << "call report_lock_point_ack_to_navigation_escort rpc ok.";
		return true;
	}
};
REGISTER_COMMAND_PARSER_ENTITY(zeg_navigation_lock_point_ack_parser, "zeg.robot.navigation.lock.point.ack");
}

#endif /* SRC_ZEG_NAVIGATION_LOCK_POINT_ACK_PARSER_HPP_ */
