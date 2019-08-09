#ifndef SRC_ZEG_ROBOT_CONFIG_HPP_
#define SRC_ZEG_ROBOT_CONFIG_HPP_
#include <sys/stat.h>
#include <stdint.h>
#include <string>
#include "NanoLog.hpp"
#include "config_parser.hpp"
#include "zeg_robot_define.hpp"
#include "udp_broadcast_client.hpp"

namespace zeg_robot_host_communication {
using namespace std;
class zeg_robot_config {
private:
	zeg_robot_config() {
		init_log();
		init_conf();
	}
	virtual ~zeg_robot_config() = default;
public:
	inline static zeg_robot_config &get_instance() {
		return config_;
	}
	bool init() {
		return true;
	}
public:
	void init_conf() {
		if (false == config_parser::config_parser::get_instance().init(conf_path)) {
			return;
		}
		get_zeg_robot_udp_server_config();
		get_rpc_config();
	}
private:
	inline void init_log() {
		mkdir(zeg_log_path, 777);
		nanolog::initialize(nanolog::GuaranteedLogger(), zeg_log_path, zeg_log_file , max_log_size);
		nanolog::set_log_level(nanolog::LogLevel::INFO);
	}
	inline void get_zeg_robot_udp_server_config() {
		vector<string>values{""};
		config_parser::config_parser::get_instance().get_value("udp_server", "port", values);
		udp_server_port = atoi(values[0].c_str());
		if (udp_server_port <= 0) {
			udp_server_port = 7780;
			LOG_INFO << "udp_server_port will be set 7780";
		}
		udp_server_broadcast_port = udp_server_port;
	}
	inline void get_rpc_config() {
		vector<string>values{""};
		config_parser::config_parser::get_instance().get_value("robot_rpc", "host_communication_port", values);
		robot_rpc_host_communication_port = atoi(values[0].c_str());
		if (robot_rpc_host_communication_port <= 0) {
			robot_rpc_host_communication_port = 9000;
			LOG_INFO << "robot_rpc_host_communication_port will be set 9000";
		}
		config_parser::config_parser::get_instance().get_value("robot_rpc", "host_entity_port", values);
		robot_rpc_host_entity_port = atoi(values[0].c_str());
		if (robot_rpc_host_entity_port <= 0) {
			robot_rpc_host_entity_port = 8000;
			LOG_INFO << "robot_rpc_host_entity_port will be set 8000";
		}
	}
private:
	static zeg_robot_config config_;
public:
	int udp_server_port;
	int udp_server_broadcast_port;
	int robot_rpc_host_communication_port;
	int robot_rpc_host_entity_port;
	string robot_id;
public:
	const char *RPC_SERVER_IP = "127.0.0.1";
	const uint32_t buffer_size = 1024;
private:
	const char *conf_path = "../params/zeg_robot_host_communication.conf";
	const char *zeg_log_path = "/hontai/log";
	const char *zeg_log_file = "zeg_robot_host_communication_log";
	const uint32_t max_log_size = 10;	// 10M
};
zeg_robot_config zeg_robot_config::config_;
}

#endif /* SRC_ZEG_ROBOT_CONFIG_HPP_ */
