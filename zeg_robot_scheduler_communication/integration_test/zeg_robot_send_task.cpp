#include <unistd.h>
#include <iostream>
#include <chrono>
#include "rpc_client.hpp"
#include "zeg_robot_define.hpp"
using namespace rest_rpc;
using namespace rpc_service;
using namespace zeg_robot_scheduler_communication;
int main() {
	rpc_client client("127.0.0.1", 11000);
	if (false == client.connect(1)) {
		cerr << "rpc server connect failed...!" << endl;
		return -1;
	}
	zeg_robot_point p0(1, 2);
	zeg_robot_point p1(11.2, 21.22);
	zeg_robot_task cmd;
	cmd.task_id = "007";
	cmd.points.emplace_back(p0);
	cmd.points.emplace_back(p1);
	while (true) {
		sleep(5);
		zeg_robot_header header("zeg.robot.task", "008", chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count());
		try {
			cout << "rpc call result = " << client.call<bool>("report_task_to_robot", header, cmd) << endl;
		}
		catch (std::exception &e) {
			cerr << e.what() << endl;
		}
	}

	return 0;
}


