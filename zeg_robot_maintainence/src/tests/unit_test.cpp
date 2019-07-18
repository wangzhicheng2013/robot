#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <thread>
#include <chrono>
#include "doctest.hpp"
#include "zeg_config.hpp"
#include "common_utility.hpp"
#include "zeg_robot_broadcast.hpp"
#include "udp_broadcast_server.hpp"
#include "udp_broadcast_client.hpp"
#include "udp_unicast_server.hpp"
#include "udp_unicast_client.hpp"
using namespace zeg_robot_maintainence;
char udp_recv_buf[BUFSIZ] = "";
TEST_CASE("testing zeg configuration") {
	CHECK(false == zeg_config::get_instance().robot_broadcast_address.empty());
	CHECK(7780 == zeg_config::get_instance().robot_broadcast_port);
	CHECK(zeg_config::get_instance().robot_broadcast_interval > 0);
}
void send_broadcast_thread(const char *buf) {
	this_thread::sleep_for(chrono::milliseconds(100));
	auto client = message_communicate_entity_maker::make_unique_ptr("udp.broadcast.client");
	if (client->init() >= 0) {
		reinterpret_cast<udp_broadcast_client *>(client.get())->set_broadcast_address(zeg_config::get_instance().robot_broadcast_address.c_str());
		client->send(buf, strlen(buf));
	}
}
void recv_broadcast_thread() {
	auto server = message_communicate_entity_maker::make_unique_ptr("udp.broadcast.server");
	if (server->init()) {
		server->recv(udp_recv_buf, sizeof(udp_recv_buf));
	}
}
TEST_CASE("testing udp broadcast entity") {
	const char *buf = "hello world, I am udp broadcast test.";
	thread th0(send_broadcast_thread, buf);
	thread th1(recv_broadcast_thread);
	th0.join();
	th1.join();
	CHECK(0 == strcmp(buf, udp_recv_buf));
}
void start_simulator() {
    run_program(zeg_config::get_instance().robot_test_simulator_path.c_str());
}
TEST_CASE("testing get vehicle ids") {
	thread th(start_simulator);
	th.detach();
	this_thread::sleep_for(chrono::milliseconds(1000));
	zeg_robot_broadcast zeg_robot_broadcast_obj;
	REQUIRE(true == zeg_robot_broadcast_obj.init());
	kill_program(zeg_config::get_instance().robot_test_simulator_name.c_str());
}
void send_unicast_thread(const char *buf, int len) {
	this_thread::sleep_for(chrono::milliseconds(200));
	auto client = message_communicate_entity_maker::make_unique_ptr("udp.unicast.client");
	if (client->init()) {
		reinterpret_cast<udp_unicast_client *>(client.get())->set_unicast_address("127.0.0.1");
		client->send(buf, len);
	}
}
void recv_unicast_thread(int &len) {
	memset(udp_recv_buf, 0, sizeof(udp_recv_buf));
	auto server = message_communicate_entity_maker::make_unique_ptr("udp.unicast.server");
	if (server->init()) {
		len = server->recv(udp_recv_buf, sizeof(udp_recv_buf));
	}
}
TEST_CASE("testing udp unicast entity") {
	string buf(1024, 'A');
	int len = 0;
	thread th0(send_unicast_thread, buf.c_str(), buf.size());
	thread th1(recv_unicast_thread, ref(len));
	th0.join();
	th1.join();
	CHECK(0 == strcmp(buf.c_str(), udp_recv_buf));
}
TEST_CASE("testing msg pack") {
	msgpack::sbuffer buffer_header;
	msgpack::sbuffer buffer_body;
	robot_basic_info body(0, 1.2, 12, 0.01, 98.899);
	msgpack::pack(buffer_body, body);
	zeg_robot_header header("zeg.robot.basic.info", "zeg_robot_xx011212DD1", chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count());
	msgpack::pack(buffer_header, header);
	char buf[1024] = "";
	memcpy(buf,  buffer_header.data(), buffer_header.size());
	memcpy(buf + buffer_header.size(),  buffer_body.data(), buffer_body.size());
	int len = buffer_header.size() + buffer_body.size();
	msgpack::unpacked msg;
	size_t offset = 0;
	msgpack::unpack(&msg, buf, len, &offset);
	cout << "offset = " << offset << endl;
	msgpack::object obj = msg.get();
	zeg_robot_header header1;
	robot_basic_info body1;
	try {
		obj.convert(&header1);
		CHECK(header.robot_id == header1.robot_id);
		CHECK(header.type == header1.type);
		CHECK(header.timestamp == header1.timestamp);
		msgpack::unpack(&msg, buf, len, &offset);
		obj = msg.get();
		obj.convert(&body1);
		CHECK(body.battery_percentage == body1.battery_percentage);
		CHECK(body.x == body1.x);
		CHECK(body.y == body1.y);
		CHECK(body.theta == body1.theta);
	}
	catch(const std::exception &e) {
		cout << e.what() << endl;
	}
}
TEST_CASE("testing udp unicast msg pack") {
	msgpack::sbuffer buffer_header;
	msgpack::sbuffer buffer_body;
	robot_basic_info body(0, 1.2, 12, 0.01, 98.899);
	msgpack::pack(buffer_body, body);
	zeg_robot_header header("zeg.robot.basic.info", "zeg_robot_xx011212DD1", chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count());
	msgpack::pack(buffer_header, header);
	char buf[1024] = "";
	memcpy(buf,  buffer_header.data(), buffer_header.size());
	memcpy(buf + buffer_header.size(),  buffer_body.data(), buffer_body.size());
	int len = buffer_header.size() + buffer_body.size();
	cout << "len = " << len << endl;
	int len1 = 0;
	thread th0(send_unicast_thread, buf, len);
	thread th1(recv_unicast_thread, ref(len1));
	th0.join();
	th1.join();
	cout << "len1 = " << len1 << endl;
	msgpack::unpacked msg;
	size_t offset = 0;
	msgpack::unpack(&msg, buf, len1, &offset);
	CHECK(len1 == len);
	msgpack::object obj = msg.get();
	zeg_robot_header header1;
	robot_basic_info body1;
	try {
		obj.convert(&header1);
		CHECK(header.robot_id == header1.robot_id);
		CHECK(header.type == header1.type);
		CHECK(header.timestamp == header1.timestamp);
		msgpack::unpack(&msg, buf, len1, &offset);
		obj = msg.get();
		obj.convert(&body1);
		CHECK(body.battery_percentage == body1.battery_percentage);
		CHECK(body.x == body1.x);
		CHECK(body.y == body1.y);
		CHECK(body.theta == body1.theta);
	}
	catch(const std::exception &e) {
		cout << e.what() << endl;
	}
}
