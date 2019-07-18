#ifndef SRC_ZEG_ROBOT_TASK_FINISH_ACK_PARSER_HPP_
#define SRC_ZEG_ROBOT_TASK_FINISH_ACK_PARSER_HPP_
#include "zeg_robot_command_parser_maker.hpp"
namespace zeg_robot_host_communication {
class zeg_robot_task_finish_ack_parser : public zeg_robot_command_parser {
public:
public:
	virtual bool parse(zeg_robot_command_unpack_struct *unpack_cmd, string &ack_str) override {
		zeg_robot_task_finish_ack cmd;
		msgpack::unpack(unpack_cmd->unpack_msg, unpack_cmd->unpack_buf, unpack_cmd->unpack_len, &unpack_cmd->unpack_offset);
		msgpack::object obj = unpack_cmd->unpack_msg->get();
		try {
			obj.convert(&cmd);
		}
		catch(const std::exception &e) {
			LOG_CRIT << e.what();
			return false;
		}
		LOG_INFO << cmd.task_id << " parse task finish ack ok.";
		return true;	// not need to send to robot
		//return call_report_task_finish_ack_to_robot(unpack_cmd, cmd);
	}
private:
	bool call_report_task_finish_ack_to_robot(const zeg_robot_command_unpack_struct *unpack_cmd, const zeg_robot_task_finish_ack &cmd) {
		static rpc_client client(zeg_robot_config::zeg_robot_config::get_instance().RPC_SERVER_IP, zeg_robot_config::get_instance().robot_rpc_host_entity_port);
		static bool connected = false;
		if (false == connected) {
			connected = client.connect(1);
		}
		bool succ = true;
		try {
			succ = client.call<bool>("report_task_finish_ack_to_robot", *(unpack_cmd->unpack_header), cmd);
		}
		catch (std::exception &e) {
			connected = false;
			succ = false;
			LOG_CRIT << e.what();
		}
		return succ;
	}
};
REGISTER_COMMAND_PARSER_ENTITY(zeg_robot_task_finish_ack_parser, "zeg.robot.task.finish.ack");
}


#endif /* SRC_ZEG_ROBOT_TASK_FINISH_ACK_PARSER_HPP_ */
