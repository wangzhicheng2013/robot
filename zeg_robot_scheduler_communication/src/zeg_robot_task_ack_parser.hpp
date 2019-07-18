#ifndef SRC_ZEG_ROBOT_TASK_ACK_PARSER_HPP_
#define SRC_ZEG_ROBOT_TASK_ACK_PARSER_HPP_
#include "zeg_robot_command_parser_maker.hpp"
#include "zeg_robot_task_escort.hpp"
namespace zeg_robot_scheduler_communication {
class zeg_robot_task_ack_parser : public zeg_robot_command_parser {
public:
	virtual bool parse(zeg_robot_command_unpack_struct *unpack_cmd, zeg_robot_process_arg &arg) override {
		zeg_robot_task_ack task_ack;
		msgpack::unpack(unpack_cmd->unpack_msg, unpack_cmd->unpack_buf, unpack_cmd->unpack_len, &unpack_cmd->unpack_offset);
		msgpack::object obj = unpack_cmd->unpack_msg->get();
		try {
			obj.convert(&task_ack);
		}
		catch(const std::exception &e) {
			LOG_CRIT << e.what();
			return false;
		}
		LOG_INFO << task_ack.task_id << " parse task ack ok.";
		zeg_robot_task_escort::get().remove_robot_task(task_ack.task_id);
		return true;
	}
};
REGISTER_COMMAND_PARSER_ENTITY(zeg_robot_task_ack_parser, "zeg.robot.task.ack");
}


#endif /* SRC_ZEG_ROBOT_TASK_ACK_PARSER_HPP_ */
