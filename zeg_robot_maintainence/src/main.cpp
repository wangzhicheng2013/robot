#include <exception>
#include "zeg_robot_broadcast.hpp"
#include "udp_unicast_server.hpp"
#include "udp_unicast_client.hpp"
#include "rpc_server.h"
using namespace zeg_robot_maintainence;
zeg_robot_broadcast zeg_robot_broadcast_thread;
bool start_thread() {
	if (false == zeg_robot_broadcast_thread.init()) {
		LOG_CRIT << "zeg_robot_broadcast_thread init failed";
		return false;
	}
	zeg_robot_broadcast_thread.run();
	return true;
}
void join_thread() {
	zeg_robot_broadcast_thread.join();
}
int main() {
	if (false == start_thread()) {
		return -1;
	}
	join_thread();

	return 0;
}



