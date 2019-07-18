#ifndef SRC_ZEG_CONFIG_HPP_
#define SRC_ZEG_CONFIG_HPP_
#include <sys/stat.h>
#include <stdint.h>
#include <string>
#include "NanoLog.hpp"
#include "config_parser.hpp"
#include "zeg_robot_define.hpp"
#include "udp_broadcast_client.hpp"

namespace zeg_message_interface {
using namespace std;
class zeg_config {
private:
	zeg_config() : udp_broadcast_client_ptr_(message_communicate_entity_maker::make_unique_ptr("udp.broadcast.client")) {
		init_log();
		init_conf();
	}
	virtual ~zeg_config() = default;
public:
	inline static zeg_config &get_instance() {
		return config_;
	}
	bool init() {
		return init_udp();
	}
public:
	void init_conf() {
		if (false == config_parser::config_parser::get_instance().init(conf_path)) {
			return;
		}
		get_udp_server_config();
		get_rpc_config();
		get_robot_config();
		get_robot_test_config();
	}
private:
	inline bool init_udp() {
		if (nullptr == udp_broadcast_client_ptr_) {
			return false;
		}
		reinterpret_cast<udp_broadcast_client *>(udp_broadcast_client_ptr_.get())->set_port(udp_server_port);
		return udp_broadcast_client_ptr_->init();
	}
	inline void init_log() {
		mkdir(zeg_log_path, 777);
		nanolog::initialize(nanolog::GuaranteedLogger(), zeg_log_path, zeg_log_file , max_log_size);
		nanolog::set_log_level(nanolog::LogLevel::INFO);
	}
	inline void get_udp_server_config() {
		vector<string>values{""};
		config_parser::config_parser::get_instance().get_value("udp_server", "port", values);
		udp_server_port = atoi(values[0].c_str());
		if (udp_server_port <= 0) {
			udp_server_port = 7780;
		}
	}
	inline void get_rpc_config() {
		vector<string>values{""};
		config_parser::config_parser::get_instance().get_value("robot_rpc", "message_interface_layer_port", values);
		robot_rpc_message_interface_layer_port = atoi(values[0].c_str());
		if (robot_rpc_message_interface_layer_port <= 0) {
			robot_rpc_message_interface_layer_port = 9001;
		}
		config_parser::config_parser::get_instance().get_value("robot_rpc", "host_layer_port", values);
		robot_rpc_host_layer_port = atoi(values[0].c_str());
		if (robot_rpc_host_layer_port <= 0) {
			robot_rpc_host_layer_port = 8000;
		}
	}
	inline void get_robot_config() {
		vector<string>values{""};
		config_parser::config_parser::get_instance().get_value("robot", "id", values);
		robot_id = values[0];
		if (robot_id.empty()) {
			LOG_CRIT << "robot id is empty.";
		}
	}
	void get_robot_test_config() {
		vector<string>values{""};
		config_parser::config_parser::get_instance().get_value("robot_test", "message_interface_path", values);
		robot_test_message_interface_path = values[0];
		if (robot_test_message_interface_path.empty()) {
			robot_test_message_interface_path = "/opt/zeg_message_interface/zeg_message_interface/bin/zeg_message_interface";
		}
		config_parser::config_parser::get_instance().get_value("robot_test", "message_interface_name", values);
		robot_test_message_interface_name = values[0];
		if (robot_test_message_interface_name.empty()) {
			robot_test_message_interface_name = "zeg_message_interface";
		}
	}
private:
	static zeg_config config_;
public:
	int udp_server_port;
	int robot_rpc_message_interface_layer_port;
	int robot_rpc_host_layer_port;

	string robot_id;
	string robot_test_message_interface_path;
	string robot_test_message_interface_name;

	unique_ptr<message_communicate_entity>udp_broadcast_client_ptr_;
public:
	const char *RPC_SERVER_IP = "127.0.0.1";
	const uint32_t buffer_size = 1024;
private:
	const char *conf_path = "../etc/zeg_message_interface.conf";
	const char *zeg_log_path = "/opt/log";
	const char *zeg_log_file = "zeg_message_interface_log";
	const uint32_t max_log_size = 100;	// 100M
};
zeg_config zeg_config::config_;
}

#endif /* SRC_ZEG_CONFIG_HPP_ */
