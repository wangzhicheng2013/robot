#ifndef SRC_ZEG_ROBOT_TASK_PARSER_HPP_
#define SRC_ZEG_ROBOT_TASK_PARSER_HPP_
#include "zeg_robot_command_parser_maker.hpp"
#include "zeg_robot_command_pack.hpp"
namespace zeg_message_interface {
class zeg_robot_task_parser : public zeg_robot_command_parser {
public:
public:
	virtual bool parse(zeg_robot_command_unpack_struct *unpack_cmd, string &ack_str) override {
		zeg_robot_task task;
		msgpack::unpack(unpack_cmd->unpack_msg, unpack_cmd->unpack_buf, unpack_cmd->unpack_len, &unpack_cmd->unpack_offset);
		msgpack::object obj = unpack_cmd->unpack_msg->get();
		try {
			obj.convert(&task);
		}
		catch(const std::exception &e) {
			LOG_CRIT << e.what();
			return false;
		}
		LOG_INFO << "zeg robot task parse ok.";
		if (true == call_report_task_to_robot(unpack_cmd, task)) {
			pack_cmd_ack(unpack_cmd, task, ack_str);
			return true;
		}
		return false;
	}
	bool call_report_task_to_robot(const zeg_robot_command_unpack_struct *unpack_cmd, const zeg_robot_task &task) {
		static rpc_client client(zeg_config::zeg_config::get_instance().RPC_SERVER_IP, zeg_config::get_instance().robot_rpc_host_layer_port);
		static bool connected = false;
		if (false == connected) {
			connected = client.connect(1);
		}
		bool succ = true;
		try {
			succ = client.call<bool>("report_task_to_robot", *(unpack_cmd->unpack_header), task);
			if (false == succ) {
				LOG_CRIT << "report_task_to_robot call rpc failed.";
			}
		}
		catch (std::exception &e) {
			connected = false;
			succ = false;
			LOG_CRIT << e.what();
		}
		return succ;
	}
private:
	 inline void pack_cmd_ack(const zeg_robot_command_unpack_struct *unpack_cmd, const zeg_robot_task &cmd, string &ack_str) {
		 zeg_robot_header header("zeg.robot.task.ack", unpack_cmd->unpack_header->robot_id, chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count());
		 zeg_robot_task_ack robot_task_ack;
		 robot_task_ack.task_id = cmd.task_id;
		 zeg_robot_command_pack<zeg_robot_task_ack>::get_instance().pack(header, robot_task_ack, ack_str);
	 }
};
REGISTER_COMMAND_PARSER_ENTITY(zeg_robot_task_parser, "zeg.robot.task");
}

#endif /* SRC_ZEG_ROBOT_TASK_PARSER_HPP_ */
