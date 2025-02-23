#ifndef SRC_ZEG_CONFIG_HPP_
#define SRC_ZEG_CONFIG_HPP_
#include <sys/stat.h>
#include <string>
#include "NanoLog.hpp"
#include "config_parser.hpp"
namespace zeg_robot_maintainence {
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
		get_broadcast_config();
		get_rpc_config();
		get_robot_test_config();
	}
private:
	inline void init_log() {
		mkdir(zeg_log_path, 777);
		nanolog::initialize(nanolog::GuaranteedLogger(), zeg_log_path, zeg_log_file , max_log_size);
		nanolog::set_log_level(nanolog::LogLevel::INFO);
	}
	inline void get_broadcast_config() {
		vector<string>values{""};
		config_parser::config_parser::get_instance().get_value("robot_broadcast", "address", values);
		robot_broadcast_address = values[0];
		if (robot_broadcast_address.empty()) {
			robot_broadcast_address = "127.0.0.1";
		}
		config_parser::config_parser::get_instance().get_value("robot_broadcast", "port", values);
		robot_broadcast_port = atoi(values[0].c_str());
		if (robot_broadcast_port <= 0) {
			robot_broadcast_port = 7780;
		}
		config_parser::config_parser::get_instance().get_value("robot_broadcast", "interval", values);
		robot_broadcast_interval = atoi(values[0].c_str());
		check_interval();
	}
	inline void get_rpc_config() {
		vector<string>values{""};
		config_parser::config_parser::get_instance().get_value("robot_rpc", "host_layer_port", values);
		robot_rpc_host_layer_port = atoi(values[0].c_str());
		if (robot_rpc_host_layer_port <= 0) {
			robot_rpc_host_layer_port = 9000;
		}
		config_parser::config_parser::get_instance().get_value("robot_rpc", "maintainence_layer_port", values);
		robot_rpc_maintainence_layer_port = atoi(values[0].c_str());
		if (robot_rpc_maintainence_layer_port <= 0) {
			robot_rpc_maintainence_layer_port = 9002;
		}
	}
	inline void get_robot_test_config() {
		vector<string>values{""};
		config_parser::config_parser::get_instance().get_value("robot_test", "simulator_path", values);
		robot_test_simulator_path = values[0];
		if (robot_test_simulator_path.empty()) {
			robot_test_simulator_path = "/opt/zeg_robot_simulator/bin/zeg_robot_simulator";
		}
		config_parser::config_parser::get_instance().get_value("robot_test", "simulator_name", values);
		robot_test_simulator_name = values[0];
		if (robot_test_simulator_name.empty()) {
			robot_test_simulator_name = "zeg_robot_simulator";
		}
	}
	inline void check_interval() {
		if (robot_broadcast_interval <= 0 || robot_broadcast_interval > 10000) {
			robot_broadcast_interval = 100;
		}
	}
private:
	static zeg_config config_;
public:
	string robot_broadcast_address;
	int robot_broadcast_interval;
	int robot_broadcast_port;

	int robot_rpc_host_layer_port;
	int robot_rpc_maintainence_layer_port;

	string robot_test_simulator_path;
	string robot_test_simulator_name;
public:
	const char *RPC_SERVER_IP = "127.0.0.1";
private:
	const char *conf_path = "../etc/zeg_robot_maintainence.conf";
	const char *zeg_log_path = "/opt/log";
	const char *zeg_log_file = "zeg_robot_maintainence_log";
	const uint32_t max_log_size = 1;    // 1M
};
zeg_config zeg_config::config_;
}

#endif /* SRC_ZEG_CONFIG_HPP_ */
