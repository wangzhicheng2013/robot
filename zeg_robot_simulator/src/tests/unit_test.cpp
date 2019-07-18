#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <iostream>
#include <fstream>
#include <random>
#include <experimental/filesystem>
#include "doctest.hpp"
#include "config_parser.hpp"
#include "zeg_pose_compute.hpp"
#include "zeg_config.hpp"
#include "rpc_client.hpp"
#include "zeg_robot_navigation.hpp"
#include "zeg_robot_poses.hpp"
#include "codec.h"
#include "mutex_deque.hpp"
using namespace std;
using namespace zeg_robot_simulator;
using namespace rest_rpc;
using namespace rest_rpc::rpc_service;
namespace fs = std::experimental::filesystem;
bool write_test_config_file(const char *filepath) {
	fstream fs(filepath, ios::out);
	if (!fs) {
		return false;
	}
	fs << "key0=v0" << endl;
	fs << "key1=v1,v2" << endl;
	fs << "[TT]" << endl;
	fs << "key2=v3" << endl;
	fs << "key4=v4" << endl;
	fs << "[TT1]" << endl;
	fs << "key2=v3" << endl;
	fs << "key4=v4" << endl;
	fs << "key5=v5" << endl;
	fs << "key6=v6,v7,v8" << endl;
	fs.close();
	return true;

}
TEST_CASE("testing configuration parser") {
	const char *filepath = "./test.conf";
	REQUIRE(true == write_test_config_file(filepath));
	REQUIRE(true == config_parser::config_parser::get_instance().init(filepath));
	CHECK(true == fs::remove(filepath));
	vector<string>values;
	config_parser::config_parser::get_instance().get_value("TT", "key2", values);
	REQUIRE(1 == values.size());
	CHECK("v3" == values[0]);

	config_parser::config_parser::get_instance().get_value("TT", "key4", values);
	REQUIRE(1 == values.size());
	CHECK("v4" == values[0]);

	config_parser::config_parser::get_instance().get_value("", "key0", values);
	REQUIRE(1 == values.size());
	CHECK("v0" == values[0]);

	config_parser::config_parser::get_instance().get_value("", "key1", values);
	REQUIRE(2 == values.size());
	CHECK("v1" == values[0]);
	CHECK("v2" == values[1]);

	config_parser::config_parser::get_instance().get_value("TT1", "key2", values);
	REQUIRE(1 == values.size());
	CHECK("v3" == values[0]);

	config_parser::config_parser::get_instance().get_value("TT1", "key6", values);
	REQUIRE(3 == values.size());
	bool succ = true;
	for (auto &v : values) {
		if ("v6" != v && "v7" != v && "v8" != v) {
			succ = false;
			break;
		}
	}
	CHECK(true == succ);
}
TEST_CASE("testing pose compute0") {
	pose_compute pose_compute_obj;
	pose_compute_obj.msecs = 1000;
	pose_compute_obj.cur_pose_ = {10, 10, 0.1};
	pose_compute_obj.speed_ = {1, 0, 1.5};
	robot_pose next_pose;
	pose_compute_obj.get_next_pose(next_pose);
	CHECK(next_pose.x > 10);
	CHECK(next_pose.y > 10);
	CHECK(next_pose.theta > 0);

	pose_compute_obj.cur_pose_ = {10, 10, 10.1};
	pose_compute_obj.speed_ = {0, 0, 0};
	pose_compute_obj.get_next_pose(next_pose);
	CHECK(10 == next_pose.x);
	CHECK(10 == next_pose.y);
	CHECK(true == equal(atan2(sin(10.1), cos(10.1)), next_pose.theta));

	pose_compute_obj.msecs = 1000;
	pose_compute_obj.cur_pose_ = {10, 0, 0};
	pose_compute_obj.speed_ = {1, 0, 0};
	pose_compute_obj.get_next_pose(next_pose);
	CHECK(11 == next_pose.x);
	CHECK(0 == next_pose.y);
	CHECK(true == equal(0.0, next_pose.theta));
}
TEST_CASE("testing pose compute1") {
	pose_compute pose_compute_obj;
	robot_pose next_pose;
	pose_compute_obj.msecs = 1000;
	pose_compute_obj.cur_pose_ = {10, 10, 0};
	pose_compute_obj.speed_ = {0, 0, 1};
	pose_compute_obj.get_next_pose(next_pose);
	CHECK(10 == next_pose.x);
	CHECK(10 == next_pose.y);
	CHECK(next_pose.theta > 0);

	pose_compute_obj.msecs = 1000;
	pose_compute_obj.cur_pose_ = {10, 10, 0};
	pose_compute_obj.speed_ = {0, 1, 0};
	pose_compute_obj.get_next_pose(next_pose);
	CHECK(10 == next_pose.x);
	CHECK(10 == next_pose.y);
	CHECK(true == equal(0.0, next_pose.theta));
}
TEST_CASE("testing get pose trace") {
	pose_compute pose_compute_obj;
	pose_compute_obj.msecs = 100;
	pose_compute_obj.cur_pose_ = {10, 10, 0};
	pose_compute_obj.destination_pose_ = {11, 10, 0};
	pose_compute_obj.speed_ = {1, 0, 0};

	vector<robot_pose>pose_trace;
	CHECK(true == pose_compute_obj.get_pose_trace(pose_trace));
	CHECK(10 == pose_trace.size());

	pose_compute_obj.cur_pose_ = {10, 10, 0.88};
	pose_compute_obj.destination_pose_ = {10, 10, 1.88};
	pose_compute_obj.speed_ = {0, 0, 1};

	CHECK(true == pose_compute_obj.get_pose_trace(pose_trace));
	pose_compute_obj.get_pose_trace(pose_trace);
	CHECK(0 == pose_trace.size());

	double x =  atan2(1.0, 1.0);
	pose_compute_obj.cur_pose_ = {10, 10, x};
	pose_compute_obj.destination_pose_ = {11, 11, 0};
	pose_compute_obj.speed_ = {1, 0, 0.1};

	CHECK(true == pose_compute_obj.get_pose_trace(pose_trace));
	CHECK(pose_trace.size() > 0);

	x =  atan2(1.0, 0.0);
	pose_compute_obj.cur_pose_ = {10, 10, x};
	pose_compute_obj.destination_pose_ = {10, 11, 0};
	pose_compute_obj.speed_ = {1, 0, 0.1};

	CHECK(true == pose_compute_obj.get_pose_trace(pose_trace));
	CHECK(pose_trace.size() > 0);

	pose_compute_obj.cur_pose_ = {10, 10, 0};
	pose_compute_obj.destination_pose_ = {10, 11, 0};
	pose_compute_obj.speed_ = {1, 0, 0.1};

	CHECK(false == pose_compute_obj.get_pose_trace(pose_trace));
}
TEST_CASE("testing adjust pose angle") {
	pose_compute pose_compute_obj;
	pose_compute_obj.msecs = 100;
	pose_compute_obj.cur_pose_ = {10, 10, 0};
	pose_compute_obj.destination_pose_ = {11, 10, 0};
	pose_compute_obj.speed_ = {1, 0, 10};
	CHECK(false == pose_compute_obj.adjust_pose_angle());

	pose_compute_obj.cur_pose_ = {10, 10, 2.1};
	pose_compute_obj.destination_pose_ = {11, 10, 10.90};
	pose_compute_obj.speed_ = {1, 0, 0.1};
	CHECK(true == pose_compute_obj.adjust_pose_angle());

	pose_compute_obj.cur_pose_ = {10, 10, 2.1};
	pose_compute_obj.destination_pose_ = {11, 10, 10.90};
	pose_compute_obj.speed_ = {1, 0, 0};
	CHECK(false == pose_compute_obj.adjust_pose_angle());
}
TEST_CASE("testing rotate robot pose") {
	pose_compute pose_compute_obj;
	pose_compute_obj.msecs = 100;
	pose_compute_obj.cur_pose_ = {10, 10, 2.1};
	pose_compute_obj.destination_pose_ = {11, 10, 10.90};
	pose_compute_obj.speed_ = {1, 0, 0.1};
	CHECK(true == pose_compute_obj.adjust_pose_angle());

	vector<robot_pose>pose_trace;
	pose_compute_obj.rotate_robot_pose(pose_trace);
	CHECK(pose_trace.size() > 0);
	CHECK(10 == pose_compute_obj.destination_pose_.y);
	CHECK(10.90 == pose_compute_obj.destination_pose_.theta);
	CHECK(1 == pose_compute_obj.speed_.vx);
}
TEST_CASE("testing merge vector") {
	vector<int>v0{1, 2, 3};
	vector<int>v1;
	merge_vector(v0, v1);
	CHECK(3 == v1.size());
	merge_vector(v0, v1);
	CHECK(6 == v1.size());
}
const int n = 2000;
TEST_CASE("testing get pose trace with angle") {
	pose_compute pose_compute_obj;
	pose_compute_obj.msecs = 100;
	pose_compute_obj.speed_ = {1, 0, 36};
	pose_compute_obj.cur_pose_ = {0, 0, 0};
	vector<robot_pose>pose_set{{1, 1, 1.57}};
	//vector<robot_pose>pose_set{{0, 1, 1.57}, {1, 1, 3.14}, {10, 1, 3.14}, {10, 1, 33.14}};
	vector<robot_pose>pose_trace;
	CHECK(true == pose_compute_obj.get_pose_trace_with_angle(pose_set, pose_trace));
	cout << "==========================" << endl;
	cout << "pose trace size = " << pose_trace.size() << endl;
	for (auto &e : pose_trace) {
		cout << "(" << e.x << "," << e.y << "," << e.theta << ")" << endl;
	}
	cout << "==========================" << endl;

}
TEST_CASE("testing get pose trace with angle1") {
	pose_compute pose_compute_obj;
	pose_compute_obj.msecs = 100;
	pose_compute_obj.speed_ = {1, 0, 0.1};
	vector<robot_pose>pose_set;
	vector<robot_pose>pose_trace;
	robot_pose tmp;
	default_random_engine e;
	uniform_real_distribution<double>u(-100, 200);
	for (int i = 0;i < n;i++) {
		robot_pose pose(3 * i + 2 * i + 1.90 / (2 * i + i - 6 * i + 1), 2 * i - 1 + 1.0 * i / 10.1, 0);
		pose_set.emplace_back(pose);
		tmp = pose;
	}
	CHECK(true == pose_compute_obj.get_pose_trace_with_angle(pose_set, pose_trace));
	REQUIRE(pose_trace.size() > 0);
	bool r = (tmp == *(end(pose_trace) - 1));
	CHECK(true == r);
	cout << "==========================" << endl;
	cout << "pose trace size = " << pose_trace.size() << endl;
	for (auto &e : pose_trace) {
		cout << "(" << e.x << "," << e.y << "," << e.theta << ")" << endl;
	}
	cout << "==========================" << endl;
}
TEST_CASE("testing get pose trace with angle2") {
	pose_compute pose_compute_obj;
	pose_compute_obj.msecs = 100;
	pose_compute_obj.speed_ = {1, 0, 0.1};
	vector<robot_pose>pose_set;
	vector<robot_pose>pose_trace;
	robot_pose tmp;
	default_random_engine e;
	uniform_real_distribution<double>u(-3.1415, 3.1890);
	for (int i = 0;i < n;i++) {
		robot_pose pose(3 * i + 2 * i + 1.90 / (2 * i + i - 6 * i + 1), 3 * i + 2 * i + 1.90 / (2 * i + i - 6 * i + 1), u(e));
		pose_set.emplace_back(pose);
		tmp = pose;
	}
	CHECK(true == pose_compute_obj.get_pose_trace_with_angle(pose_set, pose_trace));
	REQUIRE(pose_trace.size() > 0);
	bool r = (tmp == *(end(pose_trace) - 1));
	CHECK(true == r);
	cout << "==========================" << endl;
	cout << "pose trace size = " << pose_trace.size() << endl;
	for (auto &e : pose_trace) {
		cout << "(" << e.x << "," << e.y << "," << e.theta << ")" << endl;
	}
	cout << "==========================" << endl;
}
TEST_CASE("testing get pose trace with angle3") {
	pose_compute pose_compute_obj;
	pose_compute_obj.msecs = 100;
	pose_compute_obj.speed_ = {1, 0, 36};
	pose_compute_obj.cur_pose_ = {0, 0, 0};
	vector<robot_pose>pose_set{{0, 1, 1.57}, {1, 1, 3.14}};
	vector<robot_pose>pose_trace;
	CHECK(true == pose_compute_obj.get_pose_trace_with_angle(pose_set, pose_trace));
	cout << "==========================" << endl;
	cout << "pose trace size = " << pose_trace.size() << endl;
	for (auto &e : pose_trace) {
		cout << "(" << e.x << "," << e.y << "," << e.theta << ")" << endl;
	}
	cout << "==========================" << endl;
}
TEST_CASE("testing get pose trace with angle4") {
	pose_compute pose_compute_obj;
	pose_compute_obj.msecs = 100;
	pose_compute_obj.speed_ = {1, 0, 78};
	vector<robot_pose>pose_set;
	vector<robot_pose>pose_trace;
	robot_pose tmp;
	default_random_engine e;
	uniform_real_distribution<double>u(-100, 100);
	uniform_real_distribution<double>u1(-3.14, 3.14);
	for (int i = 0;i < n;i++) {
		robot_pose pose(u(e), u(e), u1(e));
		pose_set.emplace_back(pose);
		tmp = pose;
	}
	CHECK(true == pose_compute_obj.get_pose_trace_with_angle(pose_set, pose_trace));
	REQUIRE(pose_trace.size() > 0);
	bool r = (tmp == *(end(pose_trace) - 1));
	CHECK(true == r);
	cout << "==========================" << endl;
	cout << "pose trace size = " << pose_trace.size() << endl;
	for (auto &e : pose_trace) {
		cout << "(" << e.x << "," << e.y << "," << e.theta << ")" << endl;
	}
	cout << "==========================" << endl;
}
static const char *ROBOT_SIMULATOR_PATH = "/opt/zeg_robot_simulator/bin/zeg_robot_simulator";
static const char *ROBOT_SIMULATOR_NAME = "zeg_robot_simulator";
void start_server() {
	run_program(ROBOT_SIMULATOR_PATH);
}
TEST_CASE("testing rest rpc get cur pose") {
	thread th(start_server);
	th.detach();
	bool no_exception = true;
    try {
    	sleep(1);
    	rpc_client client("127.0.0.1", zeg_config::get_instance().RPC_SERVER_ROBOT_SIMULATOR_PORT);
    	bool r = client.connect();
    	REQUIRE(true == r);
        auto pose = client.call<robot_pose>("get_cur_pose");
        CHECK(robot_pose(0, 0, 0) == pose);
    }
    catch (const std::exception& e) {
    	cout << e.what() << std::endl;
    	no_exception = false;
    }
    CHECK(true == no_exception);
}
TEST_CASE("testing rest rpc get pose trace") {
	bool no_exception = true;
    try {
    	sleep(1);
    	rpc_client client("127.0.0.1", zeg_config::get_instance().RPC_SERVER_ROBOT_SIMULATOR_PORT);
    	bool r = client.connect();
    	REQUIRE(true == r);
    	vector<robot_pose>pose_set;
    	robot_pose tmp;
    	default_random_engine e;
    	uniform_real_distribution<double>u(-M_PI_, M_PI_);
    	for (int i = 0;i < n;i++) {
    		robot_pose pose(i + 1, i * 2 + 1, u(e));
    		pose_set.emplace_back(pose);
    		tmp = pose;
    	}
        auto pose_trace = client.call<vector<robot_pose>>("get_pose_trace", pose_set);
       	REQUIRE(pose_trace.size() > 0);
       	r = (tmp == *(end(pose_trace) - 1));
        CHECK(true == r);
    	cout << "==========================" << endl;
    	cout << "pose trace size = " << pose_trace.size() << endl;
    	for (auto &e : pose_trace) {
    		cout << "(" << e.x << "," << e.y << "," << e.theta << ")" << endl;
    	}
    	cout << "==========================" << endl;
        auto msecs = client.call<int>("get_robot_msecs");
        CHECK(msecs == zeg_config::get_instance().msecs);
    }
    catch (const std::exception& e) {
    	cout << e.what() << std::endl;
    	no_exception = false;
    }
    CHECK(true == no_exception);
}
TEST_CASE("testing init conf") {
	zeg_config::get_instance().init_conf();
	CHECK(100 == zeg_config::get_instance().msecs);
	CHECK(1 == zeg_config::get_instance().vechile_num);
	bool r = (zeg_config::get_instance().speed_ == robot_speed{1, 0, 90});
	CHECK(true == r);
	r = (zeg_config::get_instance().cur_pose_ == robot_pose{0, 0, 0});
	CHECK(true == r);
}
TEST_CASE("testing get_pose_trace_with_angle with target pose") {
	pose_compute pose_compute_obj;
	pose_compute_obj.msecs = 100;
	pose_compute_obj.speed_ = {1, 0, 36};
	pose_compute_obj.cur_pose_ = {0, 0, 0};
	robot_pose target_pose{10, 1, 33.14};
	vector<robot_pose>pose_trace;
	CHECK(true == pose_compute_obj.get_pose_trace_with_angle(target_pose, pose_trace));
	cout << "==========================" << endl;
	cout << "pose trace size = " << pose_trace.size() << endl;
	for (auto &e : pose_trace) {
		cout << "(" << e.x << "," << e.y << "," << e.theta << ")" << endl;
	}
	cout << "==========================" << endl;
}
TEST_CASE("testing robot vehicle move") {
	zeg_config::get_instance().init();
	zeg_robot_vehicle zeg_robot_vehicle_obj;
	zeg_robot_vehicle_obj.vehicle_cur_pose_ = {0, 0, 0};
	zeg_robot_vehicle_obj.vehicle_speed_ = {1, 0, 0};
	zeg_robot_vehicle_obj.move();
	CHECK(0.1 == zeg_robot_vehicle_obj.vehicle_cur_pose_.x);
	CHECK(0 == zeg_robot_vehicle_obj.vehicle_cur_pose_.y);
	CHECK(0 == zeg_robot_vehicle_obj.vehicle_cur_pose_.theta);
}
TEST_CASE("testing robot vehicle rotate") {
	zeg_robot_vehicle zeg_robot_vehicle_obj;
	zeg_robot_vehicle_obj.vehicle_cur_pose_ = {0, 0, 0};
	zeg_robot_vehicle_obj.vehicle_speed_ = {0, 0, 1.5708};
	zeg_robot_vehicle_obj.rotate();
	CHECK(0 == zeg_robot_vehicle_obj.vehicle_cur_pose_.x);
	CHECK(0 == zeg_robot_vehicle_obj.vehicle_cur_pose_.y);
	CHECK(equal(0.15708, zeg_robot_vehicle_obj.vehicle_cur_pose_.theta));
}
TEST_CASE("testing robot navigation upload cur pose") {
	zeg_robot_navigation zeg_robot_navigation_obj;
	CHECK(true == zeg_robot_navigation_obj.init());
	for (int i = 0;i < 1;i++) {
	//	CHECK(true == zeg_robot_navigation_obj.upload_cur_pose());
	}
}
TEST_CASE("testing update_robot_poses") {
	vector<robot_pose>v{{0, 0, 0}};
	zeg_robot_poses::get_instance().update_robot_poses(v);
	CHECK(1 == zeg_robot_poses::get_instance().id_poses_[0].size());
	deque<robot_pose>&dq = zeg_robot_poses::get_instance().id_poses_[0];
	robot_pose pose = dq.front();
	dq.pop_front();
	bool r = (robot_pose(0, 0, 0) == pose);
	CHECK(true == r);

	v.emplace_back(robot_pose(1, 1, 1));
	zeg_robot_poses::get_instance().update_robot_poses(v);
	CHECK(2 == zeg_robot_poses::get_instance().id_poses_[0].size());
	deque<robot_pose>&dq1 = zeg_robot_poses::get_instance().id_poses_[0];
	pose = dq1.front();
	dq.pop_front();
	r = (robot_pose(0, 0, 0) == pose);
	CHECK(true == r);
}
TEST_CASE("testing get robot pose") {
	robot_pose pose;
	CHECK(true == zeg_robot_poses::get_instance().get_robot_pose(pose));
	bool r = (robot_pose(1, 1, 1) == pose);
	CHECK(true == r);
	CHECK(false == zeg_robot_poses::get_instance().get_robot_pose(pose));
}
TEST_CASE("testing post poses to simulator") {
	bool no_exception = true;
    try {
    	sleep(1);
    	rpc_client client("127.0.0.1", zeg_config::get_instance().RPC_SERVER_ROBOT_SIMULATOR_PORT);
    	bool r = client.connect();
    	REQUIRE(true == r);
    	vector<robot_pose>way_points{{0, 0, 0}, {1, 1, 1}};
    	r = client.call<bool>("report_poses_to_simulator", way_points, 0);
    	CHECK(true == r);
    }
    catch (const std::exception& e) {
    	cout << e.what() << std::endl;
    	no_exception = false;
    }
    CHECK(true == no_exception);
}
TEST_CASE("testing post poses to simulator1") {
	bool no_exception = true;
    try {
    	sleep(1);
    	rpc_client client("127.0.0.1", zeg_config::get_instance().RPC_SERVER_ROBOT_SIMULATOR_PORT);
    	bool r = client.connect();
    	REQUIRE(true == r);
    	vector<robot_pose>way_points;
    	r = client.call<bool>("report_poses_to_simulator", way_points, 0);
    	CHECK(false == r);
    }
    catch (const std::exception& e) {
    	cout << e.what() << std::endl;
    	no_exception = false;
    }
    CHECK(true == no_exception);
    kill_program(ROBOT_SIMULATOR_NAME);
}
TEST_CASE("testing mutex_deque") {
	mutex_deque<int>q;

	for (int i = 0;i < n;i++) {
		q.push_back(i);
	}
	int e = -1;
	q.pop_front(e);
	CHECK(0 == e);
}
mutex_deque<string>global_queue;
void push_mutex_deque() {
	const int size = 2048;
	const int loop = 100000;
	string str(size, 'A');
	for (int i = 0;i < loop;i++) {
		global_queue.push_back(str);
	}
}
void pop_mutex_deque() {
	const int loop = 100000;
	string str;
	for (int i = 0;i < loop;i++) {
		global_queue.pop_front(str);
	}
}
TEST_CASE("testing mutex_deque performance") {
	vector<thread>producer_threads;
	vector<thread>consumer_threads;
	for (int i = 0;i < 4;i++) {
		producer_threads.emplace_back(thread(push_mutex_deque));
		consumer_threads.emplace_back(thread(pop_mutex_deque));
	}
	long long int begin_count = chrono::high_resolution_clock::now().time_since_epoch().count();
	for (auto &th : producer_threads) {
		if (th.joinable()) {
			th.join();
		}
	}
	for (auto &th : consumer_threads) {
		if (th.joinable()) {
			th.join();
		}
	}
    long long int end_count = chrono::high_resolution_clock::now().time_since_epoch().count();
    cout << "elapse time = " << (double)(end_count - begin_count) / double(1000000) << " ms" << endl;
}
TEST_CASE("testing need to rotate") {
	zeg_robot_navigation zeg_robot_navigation_obj;
	zeg_robot_navigation_obj.target_pose_ = {0, 1, 0};
	CHECK(true == zeg_robot_navigation_obj.need_to_rotate());

	zeg_robot_navigation_obj.target_pose_ = {10, 0, 0};
	CHECK(false == zeg_robot_navigation_obj.need_to_rotate());

	zeg_robot_navigation_obj.target_pose_ = {0.1, 0.001, 0};
	CHECK(false == zeg_robot_navigation_obj.need_to_rotate());
}
TEST_CASE("testing status machine change") {
	zeg_config::get_instance().init();
	vector<robot_pose>pose_set;
	zeg_robot_navigation zeg_robot_navigation_obj;
	zeg_robot_navigation_obj.vehicle_.vehicle_cur_pose_ = {0};
	zeg_robot_navigation_obj.target_pose_ = {18, -11, 1.2};
	double d = distance(zeg_robot_navigation_obj.vehicle_.vehicle_cur_pose_, zeg_robot_navigation_obj.target_pose_);
	cout << "=====================================" << endl;
	while (!equal(d, 0.0)) {
		zeg_robot_navigation_obj.status_machine_change();
		pose_set.emplace_back(zeg_robot_navigation_obj.vehicle_.vehicle_cur_pose_);
		cout << "(" << zeg_robot_navigation_obj.vehicle_.vehicle_cur_pose_.x << "," << zeg_robot_navigation_obj.vehicle_.vehicle_cur_pose_.y << "," << zeg_robot_navigation_obj.vehicle_.vehicle_cur_pose_.theta << ")" << endl;
		d = distance(zeg_robot_navigation_obj.vehicle_.vehicle_cur_pose_, zeg_robot_navigation_obj.target_pose_);
	}
	cout << "=====================================" << endl;
	cout << "size = " << pose_set.size() << endl;
	REQUIRE(pose_set.size() > 0);
	bool r = equal(distance(*rbegin(pose_set), zeg_robot_navigation_obj.target_pose_), 0.0);
	CHECK(true == r);
}
TEST_CASE("testing navigate") {
	vector<robot_pose>way_points{{1, 2, 0.1}, {3, 8, 1.2}, {80, 100, 3.11}};
	vector<robot_pose>pose_set;
	zeg_robot_navigation zeg_robot_navigation_obj;
	zeg_robot_navigation_obj.update_way_points(way_points);
	cout << "=====================================" << endl;
	while (true) {
		zeg_robot_navigation_obj.navigate();
		pose_set.emplace_back(zeg_robot_navigation_obj.vehicle_.vehicle_cur_pose_);
		cout << "(" << zeg_robot_navigation_obj.vehicle_.vehicle_cur_pose_.x << "," << zeg_robot_navigation_obj.vehicle_.vehicle_cur_pose_.y << "," << zeg_robot_navigation_obj.vehicle_.vehicle_cur_pose_.theta << ")" << endl;
		double d = distance(zeg_robot_navigation_obj.vehicle_.vehicle_cur_pose_, zeg_robot_navigation_obj.target_pose_);
		if (equal(d, 0.0)) {
			CHECK(0 != zeg_robot_navigation_obj.target_pose_.theta);
			if (zeg_robot_navigation_obj.way_points_.empty()) {
				break;
			}
		}
	}
	cout << "=====================================" << endl;
	cout << "size = " << pose_set.size() << endl;
	REQUIRE(pose_set.size() > 0);
	bool r = equal(distance(*rbegin(pose_set), zeg_robot_navigation_obj.target_pose_), 0.0);
	CHECK(true == r);
}
