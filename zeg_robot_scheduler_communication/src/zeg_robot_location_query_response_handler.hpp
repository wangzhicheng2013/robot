#ifndef SRC_ZEG_ROBOT_LOCATION_QUERY_RESPONSE_HANDLER_HPP_
#define SRC_ZEG_ROBOT_LOCATION_QUERY_RESPONSE_HANDLER_HPP_
#include "response_handler.hpp"
#include "zeg_robot_communicate_operation.hpp"
namespace zeg_robot_scheduler_communication {
class zeg_robot_location_query_response_handler: public response_handler
{
public:
	zeg_robot_location_query_response_handler() = default;
	virtual ~zeg_robot_location_query_response_handler() = default;
	virtual void handle_request(http_config &config) override {
		string str = "{\"id\":\"007\",\"pointId\":\"uuid\"}";
		config.response_body = str;
		config.response_code = HTTP_200;
	}
};
}

#endif /* SRC_ZEG_ROBOT_LOCATION_QUERY_RESPONSE_HANDLER_HPP_ */
