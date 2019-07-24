#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <stdio.h>
#include <time.h>
#include <iostream>
#include <string>
#include <queue>
#include <thread>
#include <chrono>
#include <random>
#include <string_view>
#include <experimental/filesystem>
#include "doctest.hpp"
#include "common_utility.hpp"
#include "zeg_robot_define.hpp"
#include "zeg_robot_config.hpp"
#include "base_thread.hpp"
#include "zeg_robot_update_address.hpp"
#include "zeg_robot_task_escort.hpp"
#include "url_mapper.hpp"
#include "zeg_robot_communicate_operation.hpp"
using namespace zeg_robot_scheduler_communication;
TEST_CASE("testing init conf") {
	CHECK(11000 == zeg_robot_config::get_instance().robot_rpc_scheduler_communication_port);
	CHECK(10000 == zeg_robot_config::get_instance().robot_rpc_tcs_port);
	CHECK(7780 == zeg_robot_config::get_instance().udp_server_port);
	CHECK(8888 == zeg_robot_config::get_instance().tcp_server_port);
	CHECK(1 == zeg_robot_config::get_instance().robot_task_escort_sleep_interval);
	CHECK(3 == zeg_robot_config::get_instance().robot_task_escort_try_cnt);
	CHECK(10 == zeg_robot_config::get_instance().robot_task_escort_message_backlog);
	CHECK(6 == zeg_robot_config::get_instance().robot_task_escort_threads);
}
TEST_CASE("testing zeg_robot_update_address update") {
	sockaddr_in client0;
	sockaddr_in client1;
	client0.sin_port = 1000;
	client1.sin_port = 10000;
	string id0 = "007";
	string id1 = "008";
	zeg_robot_update_address::get().update(id0, client0);
	zeg_robot_update_address::get().update(id1, client1);
	CHECK(2 == zeg_robot_update_address::get().get_client_num());
	sockaddr_in client3;
	client3.sin_port = 10001;
	string id3 = "009";
	zeg_robot_update_address::get().update(id3, client3);
	CHECK(3 == zeg_robot_update_address::get().get_client_num());
	client3.sin_addr.s_addr = 2001;
	zeg_robot_update_address::get().update(id3, client3);
	CHECK(3 == zeg_robot_update_address::get().get_client_num());
}
TEST_CASE("testing zeg_robot_update_address get") {
	sockaddr_in client0;
	sockaddr_in client1;
	client0.sin_port = 1000;
	client1.sin_port = 10000;
	string id0 = "007";
	string id1 = "008";
	zeg_robot_update_address::get().update(id0, client0);
	zeg_robot_update_address::get().update(id1, client1);
	const struct sockaddr_in *p = zeg_robot_update_address::get().get(id0);
	CHECK(1000 == p->sin_port);
}
TEST_CASE("testing zeg_robot_update_address get") {
	sockaddr_in client0;
	sockaddr_in client1;
	client0.sin_port = 1000;
	client1.sin_port = 10000;
	string id0 = "007";
	string id1 = "008";
	string id2 = "111";
	zeg_robot_update_address::get().update(id0, client0);
	zeg_robot_update_address::get().update(id1, client1);
	const struct sockaddr_in *p = zeg_robot_update_address::get().get(id0);
	CHECK(1000 == p->sin_port);
	p = zeg_robot_update_address::get().get(id2);
	CHECK(nullptr == p);
}
TEST_CASE("testing zeg_robot_task_escort") {
	string task_id = "10001";
	string pack_str = "helloworld";
	zeg_robot_task_escort::get().store_robot_task(task_id, pack_str);
	string str0;
	CHECK(true == zeg_robot_task_escort::get().get_task_pack_str(task_id, str0));
	CHECK(str0 == pack_str);
	string str1;
	zeg_robot_task_escort::get().get_taskid(str1);
	CHECK(str1 == task_id);
	zeg_robot_task_escort::get().remove_robot_task(task_id);
	CHECK(false == zeg_robot_task_escort::get().get_task_pack_str(task_id, str0));
}
TEST_CASE("testing url_mapper") {
	string url = "/robot/location?id=007";
	CHECK(ZEG_ROBOT_LOCATION == url_mapper::get().get_type(url));
	url = "/robot/location1?id=007";
	CHECK(UNKNOWN_ZEG_ROBOT_TYPE == url_mapper::get().get_type(url));
}
struct person {
	string name;
	int age;
};
REFLECTION(person, name, age)
TEST_CASE("testing iguana parse json") {
	person p;
	p.name = "hello";
	p.age = 18;
	iguana::string_stream ss;
	iguana::json::to_json(ss, p);
	cout << ss.str() << endl;
	const char *str = "{\"name\":\"hello\",\"age\":18}";
	CHECK(0 == ss.str().compare(str));

	person p1;
	iguana::json::from_json0(p1, str);
	CHECK(18 == p1.age);
	CHECK("hello" == p1.name);
}
TEST_CASE("testing parse_robot_get_location") {
	string json = "{\"id\":\"007\",\"pointId\":\"uuid\"}";
	zeg_robot_get_location get_location;
	CHECK(true == zeg_robot_communicate_operation::get().parse_robot_get_location(json, get_location));
	CHECK("007" == get_location.id);
	CHECK("uuid" == get_location.pointId);
}
