#ifndef SRC_ZEG_ROBOT_TASK_FINISH_PARSER_HPP_
#define SRC_ZEG_ROBOT_TASK_FINISH_PARSER_HPP_
#include "common_utility.hpp"
#include "zeg_robot_command_parser_maker.hpp"
#include "zeg_robot_command_pack.hpp"
namespace zeg_robot_scheduler_communication {
class zeg_robot_task_finish_parser : public zeg_robot_command_parser {
public:
public:
	virtual bool parse(zeg_robot_command_unpack_struct *unpack_cmd, zeg_robot_process_arg &arg) override {
		zeg_robot_task_finish task_finish;
		msgpack::unpack(unpack_cmd->unpack_msg, unpack_cmd->unpack_buf, unpack_cmd->unpack_len, &unpack_cmd->unpack_offset);
		msgpack::object obj = unpack_cmd->unpack_msg->get();
		try {
			obj.convert(&task_finish);
		}
		catch(const std::exception &e) {
			LOG_CRIT << e.what();
			return false;
		}
		LOG_INFO << "zeg robot task finish parse ok.";
		if (true == call_report_task_finish_to_tcs(unpack_cmd, task_finish)) {
			pack_cmd_ack(unpack_cmd, task_finish, arg.ack_str);
		}
		return true;
	}
	bool call_report_task_finish_to_tcs(const zeg_robot_command_unpack_struct *unpack_cmd, const zeg_robot_task_finish &task_finish) {
		static rpc_client client(zeg_robot_config::zeg_robot_config::get_instance().RPC_SERVER_IP, zeg_robot_config::get_instance().robot_rpc_tcs_port);
		static bool connected = false;
		if (false == connected) {
			connected = client.connect(1);
		}
		bool succ = true;
		try {
			succ = client.call<bool>("report_task_finish_to_sheduler", *(unpack_cmd->unpack_header), task_finish);
			if (false == succ) {
				LOG_CRIT << "report_task_finish_to_sheduler call rpc failed.";
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
	 inline void pack_cmd_ack(const zeg_robot_command_unpack_struct *unpack_cmd, const zeg_robot_task_finish &task_finish, string &ack_str) {
		 zeg_robot_header header("zeg.robot.task.finish.ack", unpack_cmd->unpack_header->robot_id, chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count());
		 zeg_robot_task_finish_ack task_finish_ack;
		 make_uuid(task_finish_ack.task_id);
		 zeg_robot_command_pack<zeg_robot_task_finish_ack>::get_instance().pack(header, task_finish_ack, ack_str);
	 }
};
REGISTER_COMMAND_PARSER_ENTITY(zeg_robot_task_finish_parser, "zeg.robot.task.finish");
}

#endif /* SRC_ZEG_ROBOT_TASK_FINISH_PARSER_HPP_ */
