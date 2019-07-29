#include "rpc_server.h"
#include "zeg_robot_task_escort.hpp"
#include "zeg_robot_udp_server.hpp"
#include "zeg_robot_resend_task_thread.hpp"
#include "zeg_robot_response_handler_info.hpp"
#include "http_server.hpp"
using namespace rest_rpc;
using namespace rpc_service;
using namespace zeg_robot_scheduler_communication;
auto G_UDP_SEVER_PTR = message_communicate_entity_maker::make_unique_ptr("zeg.robot.udp.server");
http_server G_HTTP_SERVER;
bool init_udp_server() {
	if (nullptr == G_UDP_SEVER_PTR) {
		return false;
	}
	socket_config config;
	config.port_ = zeg_robot_config::get_instance().udp_server_port;
	reinterpret_cast<zeg_robot_udp_server *>(G_UDP_SEVER_PTR.get())->set_config(config);
	if (false == G_UDP_SEVER_PTR->init()) {
		return false;
	}
	return true;
}
bool report_task_to_robot(rpc_conn conn, const zeg_robot_header &header, const zeg_robot_task &robot_task) {
	string pack_str;
	zeg_robot_command_pack<zeg_robot_task>::get_instance().pack(header, robot_task, pack_str);
	auto client_ptr = zeg_robot_update_address::get().get(header.robot_id);
	if (nullptr == client_ptr) {
		LOG_CRIT << "robot id = " << header.robot_id << " get client address failed.";
		return false;
	}
	int size = reinterpret_cast<zeg_robot_udp_server *>(G_UDP_SEVER_PTR.get())->send_to_client(pack_str.c_str(), pack_str.size(), *client_ptr);
	LOG_INFO << "send task to robot id = " << header.robot_id << " robot ip = " << inet_ntoa(client_ptr->sin_addr) << ":" << client_ptr->sin_port;
	zeg_robot_task_escort::get().store_robot_task(robot_task.task_id, pack_str);
	LOG_INFO << "robot id = " << header.robot_id << " send zeg robot task size = " << size;
	if (pack_str.size() != size) {
		LOG_CRIT << "send size = " << size << " need send size = " << pack_str.size();
	}
	return true;
}
bool report_point_lock_ack_to_robot(rpc_conn conn, const zeg_robot_header &header, const zeg_robot_point_lock_ack &point_lock_ack) {
	string pack_str;
	zeg_robot_command_pack<zeg_robot_point_lock_ack>::get_instance().pack(header, point_lock_ack, pack_str);
	auto client_ptr = zeg_robot_update_address::get().get(header.robot_id);
	if (nullptr == client_ptr) {
		LOG_CRIT << "robot id = " << header.robot_id << " get client address failed.";
		return false;
	}
	int size = reinterpret_cast<zeg_robot_udp_server *>(G_UDP_SEVER_PTR.get())->send_to_client(pack_str.c_str(), pack_str.size(), *client_ptr);
	LOG_INFO << "send task to robot id = " << header.robot_id << " robot ip = " << inet_ntoa(client_ptr->sin_addr) << ":" << client_ptr->sin_port;
	LOG_INFO << "robot id = " << header.robot_id << " send zeg robot point_lock_ack size = " << size;
	if (pack_str.size() != size) {
		LOG_CRIT << "send size = " << size << " need send size = " << pack_str.size();
	}
	return true;
}
void start_resend_task_threads(vector<zeg_robot_resend_task_thread>&zeg_robot_resend_task_threads) {
	int size = zeg_robot_config::get_instance().robot_task_escort_threads;
	for (int i = 0;i < size;i++) {
		zeg_robot_resend_task_threads.emplace_back(zeg_robot_resend_task_thread());
		zeg_robot_resend_task_threads[i].set_udp_ptr(reinterpret_cast<zeg_robot_udp_server *>(G_UDP_SEVER_PTR.get()));
	}
	for (auto &th : zeg_robot_resend_task_threads) {
		th.run();
	}
}
void end_resend_task_threads(vector<zeg_robot_resend_task_thread>&zeg_robot_resend_task_threads) {
	for (auto &th : zeg_robot_resend_task_threads) {
		th.join();
	}
}
bool start_http_server() {
	zeg_robot_response_handler_info::get().init();
	socket_config config;
	config.port_ = 8888;
	config.type_ = TCP;
	if (false == G_HTTP_SERVER.init(config)) {
		LOG_CRIT << "http server init failed...!";
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
		LOG_CRIT << "zeg robot udp server init failed...!";
		return -1;
	}
	if (false == start_http_server()) {
		return -1;
	}
	vector<zeg_robot_resend_task_thread>zeg_robot_resend_task_threads;
	start_resend_task_threads(zeg_robot_resend_task_threads);
	rpc_server server(zeg_robot_config::get_instance().robot_rpc_scheduler_communication_port, thread::hardware_concurrency(), 0, 1);
	server.register_handler("report_task_to_robot", report_task_to_robot);
	server.register_handler("report_point_lock_ack_to_robot", report_point_lock_ack_to_robot);
	server.run();
	end_resend_task_threads(zeg_robot_resend_task_threads);

	return 0;
}
