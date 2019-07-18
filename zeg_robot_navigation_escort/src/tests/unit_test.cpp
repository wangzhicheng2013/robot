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
#include "rpc_client.hpp"
#include "codec.h"
#include "common_utility.hpp"
#include "zeg_robot_define.hpp"
#include "zeg_config.hpp"
#include "zeg_robot_new_task_loader.hpp"
#include "zeg_robot_new_task_executor.hpp"
#include "zeg_robot_new_task_navigation.hpp"
using namespace rest_rpc;
using namespace rpc_service;
using namespace zeg_robot_navigation_escort;
namespace fs = experimental::filesystem;
TEST_CASE("testing init conf") {
	CHECK(9003 == zeg_config::get_instance().robot_rpc_navigation_escort_layer_port);
	CHECK(9000 == zeg_config::get_instance().robot_rpc_robot_host_layer_port);
	CHECK(9001 == zeg_config::get_instance().robot_rpc_message_interface_layer_port);
}
void start_navigation_escort() {
    run_program(zeg_config::get_instance().robot_test_navigation_escort_path.c_str());
}
TEST_CASE("testing report_poses_to_navigation_escort") {
	thread th(start_navigation_escort);
	th.detach();
	this_thread::sleep_for(chrono::milliseconds(6000));
	rpc_client client(zeg_config::zeg_config::get_instance().RPC_SERVER_IP, zeg_config::get_instance().robot_rpc_navigation_escort_layer_port);
	CHECK(true == client.connect(1));
	zeg_robot_header header;
	zeg_robot_navigation_command cmd;
	CHECK(true == client.call<bool>("report_poses_to_navigation_escort", header, cmd));
}
TEST_CASE("testing report_lock_ack_point_to_navigation_escort") {
	rpc_client client(zeg_config::zeg_config::get_instance().RPC_SERVER_IP, zeg_config::get_instance().robot_rpc_navigation_escort_layer_port);
	CHECK(true == client.connect(1));
	zeg_robot_header header;
	zeg_robot_navigation_lock_point cmd;
	CHECK(true == client.call<bool>("report_lock_point_ack_to_navigation_escort", header, cmd));
	kill_program(zeg_config::get_instance().robot_test_navigation_escort_name.c_str());
}
TEST_CASE("test make_uuid") {
	string uuid;
	string uuid1;
	make_uuid(uuid);
	make_uuid(uuid1);
	CHECK(false == uuid.empty());
	CHECK(false == uuid1.empty());
	CHECK(uuid != uuid1);
}
void load_new_task_thread() {
	zeg_robot_header header("zeg.robot.navigation.command", "007", chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count());
	zeg_robot_point p0(1, 2);
	zeg_robot_point p1(11.2, 21.22);
	zeg_robot_navigation_command cmd;
	cmd.task_id = "007";
	cmd.points.emplace_back(p0);
	cmd.points.emplace_back(p1);
	zeg_robot_new_task_loader::get_instance().load(header, cmd);
}
void execute_new_task_thread() {
	auto &mutex_ = zeg_robot_new_task_loader::get_instance().mutex_;
	auto &cond_ = zeg_robot_new_task_loader::get_instance().cond_;
	auto &new_task_ = zeg_robot_new_task_loader::get_instance().new_task_;
	unique_lock<mutex>lk(mutex_);
	cond_.wait(lk, [&new_task_] {return true == new_task_;});
	new_task_ = false;
}
TEST_CASE("test zeg_robot_new_task_loader load") {
	thread th0(load_new_task_thread);
	thread th1(execute_new_task_thread);
	th0.join();
	th1.join();
	CHECK(false == zeg_robot_new_task_loader::get_instance().new_task_);
	CHECK("007" == zeg_robot_new_task_loader::get_instance().cur_task_.robot_id_);
	CHECK(2 == zeg_robot_new_task_loader::get_instance().cur_task_.unsure_lock_points_.size());
	bool r = (zeg_robot_new_task_loader::get_instance().cur_task_.unsure_lock_points_[0] == zeg_robot_point(1, 2));
	CHECK(true == r);
	r = (zeg_robot_new_task_loader::get_instance().cur_task_.unsure_lock_points_[1] == zeg_robot_point(11.2, 21.22));
	CHECK(true == r);
}
TEST_CASE("test make_lock_points") {
	zeg_robot_point point(1, 2);
	zeg_robot_navigation_lock_point lock_point;
	zeg_robot_new_task_executor executor;
	for (int i = 0;i < 10;i++) {
		executor.make_lock_point_with_one_for_one_policy(point, lock_point);
	}
	CHECK(1 == lock_point.locked_points.size());
	CHECK(false == lock_point.task_id.empty());
	bool r = (lock_point.locked_points[0] == point);
	CHECK(true == r);
}
TEST_CASE("test load_unsure_lock_points") {
	zeg_navigation_escort_task escort_task;
	vector<zeg_robot_point>points{{1, 2}, {11, 22}};
	for (int i = 0;i < 10;i++) {
		escort_task.load_unsure_lock_points(points);
		CHECK(2 == escort_task.unsure_lock_points_.size());
		CHECK(2 == escort_task.sure_status_.size());
		bool r = (escort_task.unsure_lock_points_[0] == points[0]);
		CHECK(true == r);
		r = (escort_task.unsure_lock_points_[1] == points[1]);
		CHECK(true == r);
		CHECK(false == escort_task.sure_status_[0]);
		CHECK(false == escort_task.sure_status_[1]);
	}
}
TEST_CASE("test update_unsure_lock_points0") {
	zeg_robot_header header("zeg.robot.navigation.command", "007", chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count());
	zeg_robot_point p0(1, 2);
	zeg_robot_point p1(11.2, 21.22);
	zeg_robot_navigation_command cmd;
	cmd.task_id = "007";
	cmd.points.emplace_back(p0);
	cmd.points.emplace_back(p1);
	zeg_robot_new_task_loader::get_instance().update_cur_task(header, cmd);

	zeg_robot_header header1("zeg.robot.navigation.lock.point.ack", "007", chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count());
	zeg_robot_navigation_lock_point_ack cmd1;
	cmd1.task_id = "007";
	cmd1.locked_points.emplace_back(p0);
	cmd1.locked_points.emplace_back(p1);

	zeg_robot_new_task_navigation::get_instance().navigate_task(header1, cmd1);
	for (auto status : zeg_robot_new_task_loader::get_instance().cur_task_.sure_status_) {
		CHECK(true == status);
	}
	CHECK(true == zeg_robot_new_task_loader::get_instance().task_is_repeated(cmd.task_id));
}
/*TEST_CASE("test update_unsure_lock_points1") {
	zeg_robot_header header("zeg.robot.navigation.command", "007", chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count());
	zeg_robot_point p0(1, 2);
	zeg_robot_point p1(11.2, 21.22);
	zeg_robot_navigation_command cmd;
	cmd.task_id = "007";
	cmd.points.emplace_back(p0);
	cmd.points.emplace_back(p1);
	zeg_robot_new_task_loader::get_instance().update_cur_task(header, cmd);
	zeg_robot_header header1("zeg.robot.navigation.lock.point.ack", "007", chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count());
	zeg_robot_navigation_lock_point_ack cmd1;
	cmd1.task_id = "007";
	cmd1.locked_points.emplace_back(p0);

	zeg_robot_new_task_navigation::get_instance().navigate_task(header1, cmd1);
	CHECK(true == zeg_robot_new_task_loader::get_instance().cur_task_.sure_status_[0]);
	CHECK(false == zeg_robot_new_task_loader::get_instance().cur_task_.sure_status_[1]);
	CHECK(true == zeg_robot_new_task_loader::get_instance().task_is_repeated(cmd.task_id));
}
*/
