#ifndef SRC_ZEG_ROBOT_CONFIG_HPP_
#define SRC_ZEG_ROBOT_CONFIG_HPP_
#include <sys/stat.h>
#include <stdint.h>
#include <string>
#include "NanoLog.hpp"
#include "config_parser.hpp"
#include "zeg_robot_define.hpp"
namespace zeg_robot_scheduler_communication {
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
		get_udp_server_config();
		get_rpc_config();
		get_robot_task_escort_config();
	}
private:
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
            LOG_INFO << "udp_server_port will be set 7780";
            udp_server_port = 7780;
        }
    }
	inline void get_rpc_config() {
		vector<string>values{""};
		config_parser::config_parser::get_instance().get_value("robot_rpc", "scheduler_communication_port", values);
		robot_rpc_scheduler_communication_port = atoi(values[0].c_str());
		if (robot_rpc_scheduler_communication_port <= 0) {
			robot_rpc_scheduler_communication_port = 11000;
			LOG_INFO << "robot_rpc_scheduler_communication_port will be set 11000";
		}
		config_parser::config_parser::get_instance().get_value("robot_rpc", "tcs_port", values);
		robot_rpc_tcs_port = atoi(values[0].c_str());
		if (robot_rpc_tcs_port <= 0) {
			robot_rpc_tcs_port = 10000;
			LOG_INFO << "robot_rpc_tcs_port will be set 10000";
		}
	}
	void get_robot_task_escort_config() {
		vector<string>values{""};
		config_parser::config_parser::get_instance().get_value("robot_task_escort", "sleep_interval", values);
		robot_task_escort_sleep_interval = atoi(values[0].c_str());
		if (robot_task_escort_sleep_interval <= 0) {
			robot_task_escort_sleep_interval = 1;
			LOG_INFO << "robot_task_escort_sleep_interval will be set 1";
		}
		config_parser::config_parser::get_instance().get_value("robot_task_escort", "try_cnt", values);
		robot_task_escort_try_cnt = atoi(values[0].c_str());
		if (robot_task_escort_try_cnt <= 0) {
			robot_task_escort_try_cnt = 3;
			LOG_INFO << "robot_task_escort_try_cnt will be set 3";
		}
		config_parser::config_parser::get_instance().get_value("robot_task_escort", "message_backlog", values);
		robot_task_escort_message_backlog = atoi(values[0].c_str());
		if (robot_task_escort_message_backlog <= 0) {
			robot_task_escort_message_backlog = 10;
			LOG_INFO << "robot_task_escort_message_backlog will be set 10";
		}
		config_parser::config_parser::get_instance().get_value("robot_task_escort", "threads", values);
		robot_task_escort_threads = atoi(values[0].c_str());
		if (robot_task_escort_threads <= 0) {
			robot_task_escort_threads = 6;
			LOG_INFO << "robot_task_escort_threads will be set 6";
		}
	}
private:
	static zeg_robot_config config_;
public:
	int udp_server_port;
	int robot_rpc_scheduler_communication_port;
	int robot_rpc_tcs_port;

	int robot_task_escort_sleep_interval;
	int robot_task_escort_try_cnt;
	int robot_task_escort_message_backlog;
	int robot_task_escort_threads;
public:
	const char *RPC_SERVER_IP = "127.0.0.1";
	const uint32_t buffer_size = 1024;
private:
	const char *conf_path = "../etc/zeg_robot_scheduler_communication.conf";
	const char *zeg_log_path = "/opt/log";
	const char *zeg_log_file = "zeg_robot_scheduler_communication_log";
	const uint32_t max_log_size = 100;	// 100M
};
zeg_robot_config zeg_robot_config::config_;
}

#endif /* SRC_ZEG_ROBOT_CONFIG_HPP_ */
