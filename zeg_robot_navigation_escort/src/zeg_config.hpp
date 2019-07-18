#ifndef SRC_ZEG_CONFIG_HPP_
#define SRC_ZEG_CONFIG_HPP_
#include <sys/stat.h>
#include <stdint.h>
#include <string>
#include <mutex>
#include "NanoLog.hpp"
#include "config_parser.hpp"
#include "zeg_robot_define.hpp"
namespace zeg_robot_navigation_escort {
using namespace std;
class zeg_config {
private:
	zeg_config() {
		init_log();
		init_conf();
	}
	virtual ~zeg_config() = default;
public:
	inline static zeg_config &get_instance() {
		return config_;
	}
public:
	void init_conf() {
		if (false == config_parser::config_parser::get_instance().init(conf_path)) {
			return;
		}
		get_rpc_config();
		get_robot_test_config();
	}
private:
	inline void init_log() {
		mkdir(zeg_log_path, 777);
		nanolog::initialize(nanolog::GuaranteedLogger(), zeg_log_path, zeg_log_file , max_log_size);
		nanolog::set_log_level(nanolog::LogLevel::INFO);
	}
	inline void get_rpc_config() {
		vector<string>values{""};
		config_parser::config_parser::get_instance().get_value("robot_rpc", "navigation_escort_layer_port", values);
		robot_rpc_navigation_escort_layer_port = atoi(values[0].c_str());
		if (robot_rpc_navigation_escort_layer_port <= 0) {
			robot_rpc_navigation_escort_layer_port = 9003;
		}
		config_parser::config_parser::get_instance().get_value("robot_rpc", "robot_host_layer_port", values);
		robot_rpc_robot_host_layer_port = atoi(values[0].c_str());
		if (robot_rpc_robot_host_layer_port <= 0) {
			robot_rpc_robot_host_layer_port = 9000;
		}
		config_parser::config_parser::get_instance().get_value("robot_rpc", "message_interface_layer_port", values);
		robot_rpc_message_interface_layer_port = atoi(values[0].c_str());
		if (robot_rpc_message_interface_layer_port <= 0) {
			robot_rpc_message_interface_layer_port = 9001;
		}
	}
	inline void get_robot_test_config() {
		vector<string>values{""};
		config_parser::config_parser::get_instance().get_value("robot_test", "navigation_escort_path", values);
		robot_test_navigation_escort_path = values[0];
		if (robot_test_navigation_escort_path.empty()) {
			robot_test_navigation_escort_path = "/opt/zeg_robot_navigation_escort/zeg_robot_navigation_escort/bin/zeg_robot_navigation_escort";
		}
		config_parser::config_parser::get_instance().get_value("robot_test", "navigation_escort_name", values);
		robot_test_navigation_escort_name = values[0];
		if (robot_test_navigation_escort_name.empty()) {
			robot_test_navigation_escort_name = "zeg_robot_navigation_escort";
		}

		config_parser::config_parser::get_instance().get_value("robot_test", "robot_host_path", values);
		robot_test_robot_host_path = values[0];
		if (robot_test_robot_host_path.empty()) {
			robot_test_robot_host_path = "/opt/zeg_robot_simulator/bin/zeg_robot_simulator";
		}
		config_parser::config_parser::get_instance().get_value("robot_test", "robot_host_name", values);
		robot_test_robot_host_name = values[0];
		if (robot_test_robot_host_name.empty()) {
			robot_test_robot_host_name = "zeg_robot_simulator";
		}
	}
private:
	static zeg_config config_;
public:
	int robot_rpc_navigation_escort_layer_port;
	int robot_rpc_robot_host_layer_port;
	int robot_rpc_message_interface_layer_port;

	string robot_test_navigation_escort_path;
	string robot_test_navigation_escort_name;
	string robot_test_robot_host_path;
	string robot_test_robot_host_name;
public:
	const char *RPC_SERVER_IP = "127.0.0.1";
	const uint32_t buffer_size = 1024;
private:
	const char *conf_path = "../etc/zeg_robot_navigation_escort.conf";
	const char *zeg_log_path = "/opt/log";
	const char *zeg_log_file = "zeg_robot_navigation_escort_log";
	const uint32_t max_log_size = 1;	// 1M
};
zeg_config zeg_config::config_;
}

#endif /* SRC_ZEG_CONFIG_HPP_ */
