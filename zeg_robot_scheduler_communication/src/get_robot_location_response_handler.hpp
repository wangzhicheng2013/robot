#ifndef SRC_GET_ROBOT_LOCATION_RESPONSE_HANDLER_HPP_
#define SRC_GET_ROBOT_LOCATION_RESPONSE_HANDLER_HPP_
#include "response_handler.hpp"
namespace zeg_robot_scheduler_communication {
class get_robot_location_response_handler: public response_handler
{
public:
	get_robot_location_response_handler() = default;
	virtual ~get_robot_location_response_handler() = default;
	virtual void handle_request(http_config &config) override {
		string str = "{\"id\":\"007\",\"pointId\":\"uuid\"}";
		config.response_body = str;
		config.response_code = HTTP_200;
	}
};
}

#endif /* SRC_GET_ROBOT_LOCATION_RESPONSE_HANDLER_HPP_ */
