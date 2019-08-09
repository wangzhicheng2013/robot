#ifndef SRC_ZEG_ROBOT_POINT_FREE_ACK_PARSER_HPP_
#define SRC_ZEG_ROBOT_POINT_FREE_ACK_PARSER_HPP_
#include "zeg_robot_command_parser_maker.hpp"
namespace zeg_robot_host_communication {
class zeg_robot_point_free_ack_parser : public zeg_robot_command_parser {
public:
public:
	virtual bool parse(zeg_robot_command_unpack_struct *unpack_cmd, string &ack_str) override {
		zeg_robot_point_free_ack cmd;
		msgpack::unpack(unpack_cmd->unpack_msg, unpack_cmd->unpack_buf, unpack_cmd->unpack_len, &unpack_cmd->unpack_offset);
		msgpack::object obj = unpack_cmd->unpack_msg->get();
		try {
			obj.convert(&cmd);
		}
		catch(const std::exception &e) {
			LOG_CRIT << "exception = " << e.what();
			return false;
		}
		LOG_INFO << cmd.task_id << " parse point free ack ok.";
		return true;
	}
};
REGISTER_COMMAND_PARSER_ENTITY(zeg_robot_point_free_ack_parser, "zeg.robot.point.free.ack");
}


#endif /* SRC_ZEG_ROBOT_POINT_FREE_ACK_PARSER_HPP_ */
