#ifndef SRC_ZEG_ROBOT_LOCATIONS_SET_RESPONSE_HANDLER_HPP_
#define SRC_ZEG_ROBOT_LOCATIONS_SET_RESPONSE_HANDLER_HPP_
#include "response_handler.hpp"
#include "zeg_robot_communicate_operation.hpp"
namespace zeg_robot_scheduler_communication {
class zeg_robot_locations_set_response_handler: public response_handler
{
public:
	zeg_robot_locations_set_response_handler() = default;
	virtual ~zeg_robot_locations_set_response_handler() = default;
	virtual void handle_request(http_config &config) override {
		LOG_INFO << "request url = " << config.url;
		LOG_INFO << "request body = " << config.request_body;
		zeg_robot_communicate_operation::get().robot_set_locations_process(config);
		LOG_INFO << "reponse body = " << config.response_body;
		LOG_INFO << "reponse code = " << config.response_code;
	}
};
}

#endif /* SRC_ZEG_ROBOT_LOCATIONS_SET_RESPONSE_HANDLER_HPP_ */
