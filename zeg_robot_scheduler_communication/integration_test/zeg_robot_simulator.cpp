#include <unistd.h>
#include <stdlib.h>
#include <iostream>
#include <chrono>
#include "udp_broadcast_client.hpp"
#include "zeg_robot_define.hpp"
#include "zeg_robot_command_pack.hpp"
using namespace zeg_robot_scheduler_communication;
int main() {
	zeg_robot_header header("zeg.robot.basic.info", "008", chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count());
	zeg_robot_basic_info body;
	body.battery_percentage = 99.99;

	body.cur_point.x = 11584;
	body.cur_point.y = -24619;

	body.last_point.x = 11584;
	body.last_point.y = -24619;

	body.next_point.x = 11584;
	body.next_point.y = -24619;

	body.cur_theta = 10.8893;

	body.state = 0;
	auto client = message_communicate_entity_maker::make_unique_ptr("udp.broadcast.client");
	if (false == client->init()) {
		cerr << "client init failed...!" << endl;
		return -1;
	}
	while (true) {
		uint64_t t = chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count();
		header.timestamp = t;
		string pack_str;
		zeg_robot_command_pack<zeg_robot_basic_info>::get_instance().pack(header, body, pack_str);
		cout << "send robot basic info size = " << client->send(pack_str.c_str(), pack_str.size()) << endl;
		sleep(2);
	}

	return 0;
}



