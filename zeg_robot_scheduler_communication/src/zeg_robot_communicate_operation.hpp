#ifndef SRC_ZEG_ROBOT_COMMUNICATE_OPERATION_HPP_
#define SRC_ZEG_ROBOT_COMMUNICATE_OPERATION_HPP_
#include "rpc_client.hpp"
#include "codec.h"
#include "common_utility.hpp"
#include "http_config.hpp"
#include "zeg_robot_rest_struct.hpp"
namespace zeg_robot_scheduler_communication {
using namespace rest_rpc;
using namespace rest_rpc::rpc_service;
class zeg_robot_communicate_operation {
public:
	static inline zeg_robot_communicate_operation &get() {
		static zeg_robot_communicate_operation obj;
		return obj;
	}
	bool parse_robot_get_location(const string &url, zeg_robot_get_location &get_location) {
		vector<string>keys;
		vector<string>values;
		get_url_key_val(url.c_str(), keys, values);
		if (keys[0] != "id" || true == values.empty()) {
			return false;
		}
		get_location.id = values[0];
		return true;
	}
	inline void make_failed_json_for_get_location(const string &robot_id, string &json) {
		char buf[128] = "";
		snprintf(buf, sizeof(buf), "{\"id\":\"%s\",\"pointId\":null}", robot_id.c_str());
		json = buf;
	}
	void robot_get_location_process(http_config &config) {
		static rpc_client client(zeg_robot_config::zeg_robot_config::get_instance().RPC_SERVER_IP, zeg_robot_config::get_instance().robot_rpc_tcs_port);
		static bool connected = false;
		if (false == connected) {
			connected = client.connect(1);
		}
		zeg_robot_get_location get_location;
		if (false == parse_robot_get_location(config.url, get_location)) {
			LOG_CRIT << config.url << " is invalid.";
			return;
		}
		try {
			get_location.pointId = client.call<string>("get_robot_location", get_location.id);
		}
		catch (std::exception &e) {
			connected = false;
			LOG_CRIT << e.what();
		}
		if (false == get_location.pointId.empty()) {
			serialize_to_json(get_location, config.response_body);
			return;
		}
		make_failed_json_for_get_location(get_location.id, config.response_body);
	}
	void robot_set_locations_process(http_config &config) {
		static rpc_client client(zeg_robot_config::zeg_robot_config::get_instance().RPC_SERVER_IP, zeg_robot_config::get_instance().robot_rpc_tcs_port);
		static bool connected = false;
		if (false == connected) {
			connected = client.connect(1);
		}
		bool succ = true;
		zeg_robot_set_locations set_locations;
		try {
			succ = deserialize_from_json(config.request_body, set_locations) && client.call<bool>("set_robot_target_points", set_locations.id, set_locations.pointIds);
		}
		catch (std::exception &e) {
			succ = false;
			connected = false;
			LOG_CRIT << e.what();
		}
		if (false == succ) {
			config.response_code = rest_error_codes::PROCESS_FAILED;
		}
	}
	void robot_task_assignment_process(http_config &config) {
		static rpc_client client(zeg_robot_config::zeg_robot_config::get_instance().RPC_SERVER_IP, zeg_robot_config::get_instance().robot_rpc_tcs_port);
		static bool connected = false;
		if (false == connected) {
			connected = client.connect(1);
		}
		static pair<bool, string>res{false, "json error or rpc error."};
		zeg_robot_task_assignment task_assignment;
		zeg_robot_task_assignment_result task_assignment_result;
		try {
			if (true == deserialize_from_json(config.request_body, task_assignment)) {
				res = client.call<pair<bool, string>>("assign_robot_task", task_assignment.id, task_assignment.type);
			}
		}
		catch (std::exception &e) {
			connected = false;
			LOG_CRIT << e.what();
		}
		task_assignment_result.id = task_assignment.id;
		if (true == res.first) {
			task_assignment_result.accepted = true;
		}
		else {
			task_assignment_result.accepted = false;
			task_assignment_result.reason = res.second;
		}
		serialize_to_json(task_assignment_result, config.response_body);
	}
	void robot_task_termination_process(http_config &config) {
		static rpc_client client(zeg_robot_config::zeg_robot_config::get_instance().RPC_SERVER_IP, zeg_robot_config::get_instance().robot_rpc_tcs_port);
		static bool connected = false;
		if (false == connected) {
			connected = client.connect(1);
		}
		bool succ = true;
		zeg_robot_task_termination task_termination;
		try {
			succ = deserialize_from_json(config.request_body, task_termination) && client.call<bool>("terminate_robot_task", task_termination.id, task_termination.status);
		}
		catch (std::exception &e) {
			succ = false;
			connected = false;
			LOG_CRIT << e.what();
		}
		if (false == succ) {
			config.response_code = rest_error_codes::PROCESS_FAILED;
		}
	}
private:
	zeg_robot_communicate_operation() = default;
	~zeg_robot_communicate_operation() = default;
};
}

#endif /* SRC_ZEG_ROBOT_COMMUNICATE_OPERATION_HPP_ */
