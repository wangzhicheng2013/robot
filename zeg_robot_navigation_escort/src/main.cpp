#include <string.h>
#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include "rpc_server.h"
#include "zeg_robot_new_task_loader.hpp"
#include "zeg_robot_new_task_executor.hpp"
#include "zeg_robot_new_task_navigation.hpp"
using namespace rest_rpc;
using namespace rpc_service;
using namespace zeg_robot_navigation_escort;
bool report_poses_to_navigation_escort(rpc_conn conn, const zeg_robot_header &header, const zeg_robot_navigation_command &cmd) {
	zeg_robot_new_task_loader::get_instance().load(header, cmd);
	return true;
}
bool report_lock_point_ack_to_navigation_escort(rpc_conn conn, const zeg_robot_header &header, const zeg_robot_navigation_lock_point_ack &cmd) {
	zeg_robot_new_task_navigation::get_instance().navigate_task(header, cmd);
	return true;
}
zeg_robot_new_task_executor zeg_robot_new_task_executor_thread;
bool start_thread() {
	if (false == zeg_robot_new_task_executor_thread.init()) {
		return false;
	}
	zeg_robot_new_task_executor_thread.run();
	return true;
}
void join_thread() {
	zeg_robot_new_task_executor_thread.join();
}
int main() {
	if (false == start_thread()) {
		return -1;
	}
	rpc_server server(zeg_config::get_instance().robot_rpc_navigation_escort_layer_port, thread::hardware_concurrency(), 0, 1);
	server.register_handler("report_poses_to_navigation_escort", report_poses_to_navigation_escort);
	server.register_handler("report_lock_point_ack_to_navigation_escort", report_lock_point_ack_to_navigation_escort);
	server.run();
	join_thread();

	return 0;
}
