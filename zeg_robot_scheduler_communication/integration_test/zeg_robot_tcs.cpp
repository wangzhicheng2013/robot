#include "rpc_server.h"
#include "zeg_robot_define.hpp"
#include "common_utility.hpp"
using namespace rest_rpc;
using namespace rpc_service;
using namespace zeg_robot_scheduler_communication;
bool report_robot_basic_info(rpc_conn conn, const zeg_robot_header &header, const zeg_robot_basic_info &cmd) {
	cout << "receive zeg robot basic info ok." << endl;
	return true;
}
bool report_point_lock_ack_to_robot(rpc_conn conn, const zeg_robot_header &header, const zeg_robot_point_lock_ack &cmd) {
	cout << "receive zeg point lock ack." << endl;
	return true;
}
bool report_task_to_robot(rpc_conn conn, const zeg_robot_header &header, const zeg_robot_task &robot_task) {
	return true;
}

bool report_point_free_ack_to_robot(rpc_conn conn, const zeg_robot_header &header, const zeg_robot_point_free_ack &cmd) {
	cout << "receive zeg point free ack." << endl;
	return true;
}
bool report_task_finish_ack_to_robot(rpc_conn conn, const zeg_robot_header &header, const zeg_robot_task_finish_ack &cmd) {
	cout << "receive zeg task finish ack." << endl;
	return true;
}
int main() {
	rpc_server server(10000, thread::hardware_concurrency(), 0, 1);
	server.register_handler("report_robot_basic_info", report_robot_basic_info);
	server.register_handler("report_task_to_robot", report_task_to_robot);
	server.register_handler("report_point_lock_ack_to_robot", report_point_lock_ack_to_robot);
	server.register_handler("report_point_free_ack_to_robot", report_point_free_ack_to_robot);
	server.register_handler("report_task_finish_ack_to_robot", report_task_finish_ack_to_robot);
	server.run();

	return 0;
}



