#ifndef SRC_ZEG_ROBOT_BASIC_INFO_PARSER_HPP_
#define SRC_ZEG_ROBOT_BASIC_INFO_PARSER_HPP_
#include "zeg_robot_command_parser_maker.hpp"
#include "zeg_robot_command_pack.hpp"
#include "zeg_robot_update_address.hpp"
#include "zeg_robot_network_message_exception_reporter.hpp"
namespace zeg_robot_scheduler_communication {
class zeg_robot_basic_info_parser : public zeg_robot_command_parser {
public:
public:
	virtual bool parse(zeg_robot_command_unpack_struct *unpack_cmd, zeg_robot_process_arg &arg) override {
		zeg_robot_basic_info basic_info;
		msgpack::unpack(unpack_cmd->unpack_msg, unpack_cmd->unpack_buf, unpack_cmd->unpack_len, &unpack_cmd->unpack_offset);
		msgpack::object obj = unpack_cmd->unpack_msg->get();
		try {
			obj.convert(&basic_info);
		}
		catch(const std::exception &e) {
			LOG_CRIT << "exception = " << e.what();
			return false;
		}
		LOG_INFO << "zeg robot basic info parse ok. robot id = " << unpack_cmd->unpack_header->robot_id;
/*		if (true == zeg_robot_network_message_exception_reporter::get().report_exception(*(unpack_cmd->unpack_header))) {
			return false;
		}*/
		auto client_ptr = static_cast<struct sockaddr_in *>(arg.client_addr);
		if (client_ptr != nullptr) {
			zeg_robot_update_address::get().update(unpack_cmd->unpack_header->robot_id, *(client_ptr));
		}
		return call_report_robot_basic_info_to_tcs(unpack_cmd, basic_info);
	}
	bool call_report_robot_basic_info_to_tcs(const zeg_robot_command_unpack_struct *unpack_cmd, const zeg_robot_basic_info &basic_info) {
		static rpc_client client(zeg_robot_config::zeg_robot_config::get_instance().RPC_SERVER_IP, zeg_robot_config::get_instance().robot_rpc_tcs_port);
		static bool connected = false;
		if (false == connected) {
			connected = client.connect(1);
		}
		bool succ = true;
		try {
			succ = client.call<bool>("report_robot_basic_info", *(unpack_cmd->unpack_header), basic_info);
			if (false == succ) {
				LOG_CRIT << "report_robot_basic_info call rpc failed.";
			}
		}
		catch (std::exception &e) {
			connected = false;
			succ = false;
			LOG_CRIT << "exception = " << e.what();
		}
		return succ;
	}
};
REGISTER_COMMAND_PARSER_ENTITY(zeg_robot_basic_info_parser, "zeg.robot.basic.info");
}

#endif /* SRC_ZEG_ROBOT_BASIC_INFO_PARSER_HPP_ */
