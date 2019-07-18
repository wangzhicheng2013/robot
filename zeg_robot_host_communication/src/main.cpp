#include "rpc_server.h"
#include "zeg_robot_command_processor.hpp"
#include "zeg_robot_udp_server.hpp"
using namespace rest_rpc;
using namespace rpc_service;
using namespace zeg_robot_host_communication;
auto udp_server_ptr = message_communicate_entity_maker::make_unique_ptr("zeg.robot.udp.server");
bool report_task_finish_to_scheduler(rpc_conn conn, const zeg_robot_header &header, const zeg_robot_task_finish &robot_task_finish) {
	string pack_str;
	zeg_robot_command_pack<zeg_robot_task_finish>::get_instance().pack(header, robot_task_finish, pack_str);
	int size = udp_server_ptr->send(pack_str.c_str(), pack_str.size());
	LOG_INFO << "send zeg robot task finish size = " << size;
	if (pack_str.size() != size) {
		LOG_CRIT << "send size = " << size << " need send size = " << pack_str.size();
		return false;
	}
	return true;
}
bool report_point_lock_to_sheduler(rpc_conn conn, const zeg_robot_header &header, const zeg_robot_point_lock &body) {
	string pack_str;
	zeg_robot_command_pack<zeg_robot_point_lock>::get_instance().pack(header, body, pack_str);
	int size = udp_server_ptr->send(pack_str.c_str(), pack_str.size());
	LOG_INFO << "send zeg robot point lock size = " << size;
	if (pack_str.size() != size) {
		LOG_CRIT << "send size = " << size << " need send size = " << pack_str.size();
		return false;
	}
	return true;
}
bool report_point_free_to_sheduler(rpc_conn conn, const zeg_robot_header &header, const zeg_robot_point_free &body) {
	string pack_str;
	zeg_robot_command_pack<zeg_robot_point_free>::get_instance().pack(header, body, pack_str);
	int size = udp_server_ptr->send(pack_str.c_str(), pack_str.size());
	LOG_INFO << "send zeg robot point free size = " << size;
	if (pack_str.size() != size) {
		LOG_CRIT << "send size = " << size << " need send size = " << pack_str.size();
		return false;
	}
	return true;
}
bool report_robot_basic_info(rpc_conn conn, const zeg_robot_header &header, const zeg_robot_basic_info &body) {
	string pack_str;
	zeg_robot_command_pack<zeg_robot_basic_info>::get_instance().pack(header, body, pack_str);
	int size = reinterpret_cast<zeg_robot_udp_server *>(udp_server_ptr.get())->send_broadcast(pack_str.c_str(), pack_str.size());
	LOG_INFO << "send robot basic info size = " << size;
	if (pack_str.size() != size) {
		LOG_CRIT << "send size = " << size << " need send size = " << pack_str.size();
		return false;
	}
	return true;
}
bool init_udp_server() {
	if (nullptr == udp_server_ptr) {
		return false;
	}
	socket_config config;
	config.port_ = zeg_robot_config::get_instance().udp_server_port;
	reinterpret_cast<zeg_robot_udp_server *>(udp_server_ptr.get())->set_config(config);
	if (false == udp_server_ptr->init()) {
		return false;
	}
	return true;
}
int main() {
	if (false == zeg_robot_config::get_instance().init()) {
		LOG_CRIT << "zeg config init failed...!";
		return -1;
	}
	if (false == init_udp_server()) {
		return -1;
	}
	rpc_server server(zeg_robot_config::get_instance().robot_rpc_host_communication_port, thread::hardware_concurrency(), 0, 1);
	server.register_handler("report_task_finish_to_scheduler", report_task_finish_to_scheduler);
	server.register_handler("report_point_lock_to_sheduler", report_point_lock_to_sheduler);
	server.register_handler("report_point_free_to_sheduler", report_point_free_to_sheduler);
	server.register_handler("report_robot_basic_info", report_robot_basic_info);
	server.run();

	return 0;
}



