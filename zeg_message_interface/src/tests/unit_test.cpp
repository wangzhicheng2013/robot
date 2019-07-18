#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <stdio.h>
#include <time.h>
#include <iostream>
#include <string>
#include <queue>
#include <thread>
#include <chrono>
#include <random>
#include <experimental/filesystem>
#include "doctest.hpp"
#include "common_utility.hpp"
#include "zeg_robot_define.hpp"
#include "zeg_config.hpp"
#include "base_thread.hpp"
#include "zeg_robot_command_processor.hpp"
#include "udp_unicast_server.hpp"
#include "udp_unicast_client.hpp"
#include "udp_server.hpp"
using namespace zeg_message_interface;
namespace fs = experimental::filesystem;
TEST_CASE("testing init conf") {
	CHECK(9001 == zeg_config::get_instance().robot_rpc_message_interface_layer_port);
	CHECK(7780 == zeg_config::get_instance().udp_server_port);
	CHECK(false == zeg_config::get_instance().robot_id.empty());
}
class my_class {
public:
    my_class() {
        my_int = 0;
    }
public:
    int my_int;
    string my_string;
public:
    MSGPACK_DEFINE(my_string, my_int);
};
TEST_CASE("testing msg pack and unpack") {
	msgpack::sbuffer buffer;
    int loop = 1000;
    char buf[1024] = "";
    for (int i = 0;i < loop;i++) {
        my_class my_obj;
        my_obj.my_int = i;
        snprintf(buf, sizeof(buf), "hello world_%d_%ld", i, time(NULL));
        my_obj.my_string = buf;
        buffer.clear();
        msgpack::pack(buffer, my_obj);

        msgpack::unpacked msg;
        msgpack::unpack(&msg, buffer.data(), buffer.size());
        msgpack::object obj = msg.get();
        my_class my_obj1;
        obj.convert(&my_obj1);
        CHECK(my_obj1.my_int == my_obj.my_int);
        CHECK(my_obj1.my_string == my_obj.my_string);
    }
    const char *str = "hello world 123ABCD edf";
    msgpack::unpacked msg;
    msgpack::unpack(&msg, str, strlen(str));
    msgpack::object obj = msg.get();
    CHECK(false == obj.is_nil());
    my_class my_obj;
    CHECK_THROWS_AS(obj.convert(&my_obj), std::exception&);
}
class add_thread : public base_thread  {
protected:
    virtual void process() override {
        sum = 0;
        for (int i = 0;i < n;i++) {
            sum += i;
        }
    }
public:
    int sum;
    int n;
};
TEST_CASE("testing thread base") {
    add_thread my_thread;
    my_thread.n = 1000000;
    my_thread.run();
    my_thread.join();
    int sum = 0;
    for (int i = 0;i < my_thread.n;i++) {
    	sum += i;
    }
    CHECK(sum == my_thread.sum);
}
TEST_CASE("testing zeg_command_processor process0") {
	const char *buf = "hello world. 12345678";
	int len = strlen(buf);
	string ack_str;
	CHECK(false == zeg_robot_command_processor::get_instance().process(buf, len, ack_str));
}
TEST_CASE("testing zeg_command_processor process1") {
	zeg_robot_header header("zeg.robot.basic.in12fo", "zeg_robot_xx011212DD1", chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count());
	zeg_robot_point p0(1, 2);
	zeg_robot_point p1(11.2, 21.22);
	zeg_robot_task cmd;
	cmd.task_id = "007";
	cmd.points.emplace_back(p0);
	cmd.points.emplace_back(p1);

	msgpack::sbuffer buffer_header;
	msgpack::sbuffer buffer_body;
	msgpack::pack(buffer_header, header);
	msgpack::pack(buffer_body, cmd);

	char buf[1024] = "";
	memcpy(buf,  buffer_header.data(), buffer_header.size());
	memcpy(buf + buffer_header.size(),  buffer_body.data(), buffer_body.size());
	int len = buffer_header.size() + buffer_body.size();
	string ack_str;
	CHECK(false == zeg_robot_command_processor::get_instance().process(buf, len, ack_str));
	CHECK(true == ack_str.empty());
}
void start_message_interface() {
	run_program(zeg_config::get_instance().robot_test_message_interface_path.c_str());
}
void pack_zeg_robot_task(string &pack_str) {
	zeg_robot_header header("zeg.robot.task", "007", chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count());
	zeg_robot_point p0(1, 2);
	zeg_robot_point p1(11.2, 21.22);
	zeg_robot_task cmd;
	cmd.task_id = "007";
	cmd.points.emplace_back(p0);
	cmd.points.emplace_back(p1);

	zeg_robot_command_pack<zeg_robot_task>::get_instance().pack(header, cmd, pack_str);
}
TEST_CASE("testing zeg_command_processor process2") {
	thread th(start_message_interface);
	th.detach();
	this_thread::sleep_for(chrono::milliseconds(6000));

	string pack_str;
	pack_zeg_robot_task(pack_str);
	string ack_str;
	CHECK(false == zeg_robot_command_processor::get_instance().process(pack_str.c_str(), pack_str.size(), ack_str));
	CHECK(true == ack_str.empty());
}
TEST_CASE("testing zeg_command_processor process3") {
	zeg_robot_header header("zeg.robot.point.lock.ack", "007", chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count());
	zeg_robot_point p0(1, 2);
	zeg_robot_point p1(11.2, 21.22);
	zeg_robot_point_lock_ack cmd;
	cmd.task_id = "007";
	cmd.locked_points.emplace_back(p0);
	cmd.locked_points.emplace_back(p1);

	string pack_str;
	zeg_robot_command_pack<zeg_robot_point_lock_ack>::get_instance().pack(header, cmd, pack_str);
	int len = pack_str.size();
	string ack_str;
	CHECK(false == zeg_robot_command_processor::get_instance().process(pack_str.c_str(), len, ack_str));
}
TEST_CASE("testing process command") {
	string pack_str;
	pack_zeg_robot_task(pack_str);
	int len = pack_str.size();

	rpc_client client(zeg_config::zeg_config::get_instance().RPC_SERVER_IP, zeg_config::get_instance().robot_rpc_message_interface_layer_port);
	CHECK(true == client.connect(1));
	bool no_exception = true;
	string ack_str;
	try {
		zeg_robot_command_processor::get_instance().process(pack_str.c_str(), len, ack_str);
	}
	catch (...) {
		no_exception = false;
	}
	CHECK(true == no_exception);
}
udp_unicast_server udp_unicast_server_obj;
udp_unicast_client udp_unicast_client_obj;
udp_unicast_server udp_unicast_schedule_server_obj;
void udp_client_thread(string &str) {
	udp_unicast_client_obj.set_port(10001);
	if (udp_unicast_client_obj.send("AAA", 3) <= 0) {
		cerr << "send AAA error." << endl;
		return;
	}
	char buf[1024] = "";
	int recv_len = udp_unicast_client_obj.recv(buf, sizeof(buf));
	if (recv_len > 0) {
		str.assign(buf, recv_len);
	}
}
void udp_server_thread(const string &str) {
	char buf[1024] = "";
	int len = sizeof(buf);
	int recv_len = udp_unicast_server_obj.recv(buf, len);
	if (recv_len > 0) {
		udp_unicast_server_obj.send(buf, recv_len);
		udp_unicast_server_obj.make_scheduler_client_addr("127.0.0.1", 10002);
		udp_unicast_server_obj.send_to_schedule_server(str.c_str(), str.size());
	}
}
void udp_schdule_server_thread(string &str) {
	char buf[1024] = "";
	int len = sizeof(buf);
	int recv_len = udp_unicast_schedule_server_obj.recv(buf, len);
	if (recv_len > 0) {
		str.assign(buf, recv_len);
	}
}
TEST_CASE("testing unicast send recv") {
	CHECK(true == udp_unicast_client_obj.init());
	udp_unicast_server_obj.set_port(10001);
	CHECK(true == udp_unicast_server_obj.init());
	udp_unicast_schedule_server_obj.set_port(10002);
	CHECK(true == udp_unicast_schedule_server_obj.init());
	string str = "hello unicast.";
	string recv_str;
	string recv_str1;
	thread th0(udp_client_thread, ref(recv_str));
	thread th1(udp_server_thread, ref(str));
	thread th2(udp_schdule_server_thread, ref(recv_str1));
	th0.join();
	th1.join();
	th2.join();
	CHECK(recv_str == "AAA");
	CHECK(recv_str1 == str);
}
TEST_CASE("testing udp server send and recv robot task") {
	string cmd_str;
	pack_zeg_robot_task(cmd_str);
	udp_unicast_client_obj.set_port(7780);
	CHECK(udp_unicast_client_obj.send(cmd_str.c_str(), cmd_str.size()) > 0);
	char buf[1024] = "";
	int recv_len = udp_unicast_client_obj.recv(buf, sizeof(buf));
	CHECK(recv_len < 0);
	kill_program(zeg_config::get_instance().robot_test_message_interface_name.c_str());
}
