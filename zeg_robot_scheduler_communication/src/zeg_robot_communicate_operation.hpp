#ifndef SRC_ZEG_ROBOT_COMMUNICATE_OPERATION_HPP_
#define SRC_ZEG_ROBOT_COMMUNICATE_OPERATION_HPP_
#include "rpc_client.hpp"
#include "codec.h"
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
	inline bool parse_robot_get_location(const string &json, zeg_robot_get_location &get_location) {
		return iguana::json::from_json0(get_location, json.c_str());
	}
private:
	zeg_robot_communicate_operation() = default;
	~zeg_robot_communicate_operation() = default;
};
}



#endif /* SRC_ZEG_ROBOT_COMMUNICATE_OPERATION_HPP_ */
