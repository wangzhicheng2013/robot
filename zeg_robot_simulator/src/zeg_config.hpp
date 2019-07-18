#ifndef SRC_ZEG_CONFIG_HPP_
#define SRC_ZEG_CONFIG_HPP_
#include <sys/stat.h>
#include "zeg_robot_define.hpp"
#include "config_parser.hpp"
#include "NanoLog.hpp"
namespace zeg_robot_simulator {
using namespace std;
class zeg_config {
private:
	zeg_config() {
		vechile_num = 1;
		msecs = 100;
		speed_ = {1, 0, 90};
	}
	virtual ~zeg_config() = default;
public:
	inline static zeg_config &get_instance() {
		return config_;
	}
	void init() {
		init_log();
		init_conf();
	}
	void init_conf() {
		if (false == config_parser::config_parser::get_instance().init(conf_path)) {
			return;
		}
		vector<string>values{""};
		config_parser::config_parser::get_instance().get_value("", "vechile_num", values);
		vechile_num = atoi(values[0].c_str());
		check_vechile_num();
		config_parser::config_parser::get_instance().get_value("", "msecs", values);
		msecs = atoi(values[0].c_str());
		check_msecs();
		config_parser::config_parser::get_instance().get_value("speed", "vx", values);
		speed_.vx = atof(values[0].c_str());
		config_parser::config_parser::get_instance().get_value("speed", "vy", values);
		speed_.vy = atof(values[0].c_str());
		config_parser::config_parser::get_instance().get_value("speed", "w", values);
		speed_.w = atof(values[0].c_str());
		cheek_speed();
		config_parser::config_parser::get_instance().get_value("cur_pose", "x", values);
		cur_pose_.x = atof(values[0].c_str());
		config_parser::config_parser::get_instance().get_value("cur_pose", "y", values);
		cur_pose_.y = atof(values[0].c_str());
		config_parser::config_parser::get_instance().get_value("cur_pose", "theta", values);
		cur_pose_.theta = atof(values[0].c_str());
	}
private:
	inline void check_msecs() {
		if (msecs <= 0 || msecs > 10000) {
			msecs = 100;
		}
	}
	inline void check_vechile_num() {
		if (vechile_num <= 0 || vechile_num > 100) {
			vechile_num = 1;
		}
	}
	inline void cheek_speed() {
		if (speed_.vx < 0 || speed_.vx >= 100) {
			speed_.vx = 1;
		}
		if (speed_.vy != 0) {
			speed_.vy = 0;
		}
		if (speed_.w < 0 || speed_.w > 100) {
			speed_.w = 0.1;
		}
	}
	inline void cheek_cur_pose() {
		if (cur_pose_.x < -1000 || cur_pose_.x > 1000) {
			cur_pose_.x = 0;
		}
		if (cur_pose_.y < -1000 || cur_pose_.y > 1000) {
			cur_pose_.y = 0;
		}
		if (cur_pose_.theta < -M_PI_ || cur_pose_.theta > M_PI_) {
			cur_pose_.theta = 0;
		}
	}
	inline void init_log() {
		mkdir(zeg_log_path, 777);
		nanolog::initialize(nanolog::GuaranteedLogger(), zeg_log_path, zeg_log_file , max_log_size);
		nanolog::set_log_level(nanolog::LogLevel::INFO);
	}
private:
	static zeg_config config_;
public:
	const int RPC_SERVER_ROBOT_SIMULATOR_PORT = 9000;
	const int RPC_SERVER_REPORT_POSE_PORT = 9001;
	const char *RPC_SERVER_IP = "127.0.0.1";
	const char *conf_path = "../etc/robot_simulator.conf";
public:
	int vechile_num;
	int msecs;
	robot_speed speed_;
	robot_pose cur_pose_;
private:
	const char *zeg_log_path = "/opt/log";
	const char *zeg_log_file = "zeg_robot_simulator_log";
	const uint32_t max_log_size = 1;	// 1M
};
zeg_config zeg_config::config_;
}

#endif /* SRC_ZEG_CONFIG_HPP_ */
