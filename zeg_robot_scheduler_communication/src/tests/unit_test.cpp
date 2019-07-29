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
#include "zeg_robot_network_message_exception_logger.hpp"
#include "common_events.hpp"
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
TEST_CASE("testing get_url_key_val") {
	const char *uri = "http://www.123.com/jk/lll?key0=val0&key1=val1&key2=val2&keyXX0=val_0090*";
	vector<string>keys;
	vector<string>values;
	get_url_key_val(uri, keys, values);
	CHECK("key0" == keys[0]);
	CHECK("key1" == keys[1]);
	CHECK("key2" == keys[2]);
	CHECK("keyXX0" == keys[3]);
	CHECK("val0" == values[0]);
	CHECK("val1" == values[1]);
	CHECK("val2" ==  values[2]);
	CHECK("val_0090*" == values[3]);
}
TEST_CASE("testing parse_robot_get_location") {
	string url = "http://127.0.0.1:8888/robot/location?id=007";
	zeg_robot_get_location get_location;
	CHECK(true == zeg_robot_communicate_operation::get().parse_robot_get_location(url, get_location));
	CHECK("007" == get_location.id);
}
TEST_CASE("testing serialize_robot_get_location") {
	zeg_robot_get_location get_location;
	get_location.id = "007";
	get_location.pointId = "uuid001";
	string json;
	serialize_to_json(get_location, json);
	CHECK(0 == json.compare("{\"id\":\"007\",\"pointId\":\"uuid001\"}"));
}
TEST_CASE("testing make_failed_json_for_get_location") {
	string robot_id = "007";
	string json;
	zeg_robot_communicate_operation::get().make_failed_json_for_get_location(robot_id, json);
	CHECK(0 == json.compare("{\"id\":\"007\",\"pointId\":null}"));
}
TEST_CASE("testing robot_get_location_process") {
	string url = "http://127.0.0.1:8888/robot/location?id=007";
	http_config config;
	config.url = url;
	zeg_robot_communicate_operation::get().robot_get_location_process(config);
	CHECK(0 == config.response_body.compare("{\"id\":\"007\",\"pointId\":null}"));
	CHECK(200 == config.response_code);
}
TEST_CASE("testing deserialize_from_json") {
	string json = "{\"id\":\"007\",\"pointIds\":[\"uuid001\",\"uuid002\"]}";
	zeg_robot_set_locations set_locations;
	CHECK(true == deserialize_from_json(json, set_locations));
	CHECK("007" == set_locations.id);
	REQUIRE(2 == set_locations.pointIds.size());
	CHECK("uuid001" == set_locations.pointIds[0]);
	CHECK("uuid002" == set_locations.pointIds[1]);
}
TEST_CASE("testing robot_set_locations_process") {
	string url = "http://127.0.0.1:8888/robot/locations";
	string json = "{\"id\":\"007\",\"pointIds\":[\"uuid001\",\"uuid002\"]}";
	http_config config;
	config.url = url;
	config.request_body = json;
	zeg_robot_communicate_operation::get().robot_set_locations_process(config);
	CHECK(rest_error_codes::PROCESS_FAILED == config.response_code);
	CHECK(true == config.response_body.empty());
}
TEST_CASE("testing robot_task_assignment_process") {
	string url = "http://127.0.0.1:8888/robot/task/assignment";
	string json = "{\"id\":\"007\",\"type\":\"picking\"}";
	http_config config;
	config.url = url;
	config.request_body = json;
	zeg_robot_communicate_operation::get().robot_task_assignment_process(config);
	CHECK(200 == config.response_code);
	CHECK(false == config.response_body.empty());
	json = "{\"id\":\"007\",\"accepted\":false,\"reason\":\"json error or rpc error.\"}";
	CHECK(0 == config.response_body.compare(json));
}
TEST_CASE("testing robot_task_termination_process") {
	string url = "http://127.0.0.1:8888/robot/task/termination";
	string json = "{\"id\":\"007\",\"status\":\"success\"}";
	http_config config;
	config.url = url;
	config.request_body = json;
	zeg_robot_communicate_operation::get().robot_task_termination_process(config);
	CHECK(rest_error_codes::PROCESS_FAILED == config.response_code);
	CHECK(true == config.response_body.empty());
}
