/*
 * response_handler_Info.hpp
 *
 *  Created on: 2019年7月23日
 *      Author: root
 */

#ifndef SRC_RESPONSE_HANDLER_INFO_HPP_
#define SRC_RESPONSE_HANDLER_INFO_HPP_
#include "http_utility.hpp"
#include "get_robot_location_response_handler.hpp"
namespace zeg_robot_scheduler_communication {
class response_handler_info {
public:
	response_handler_info() = default;
	virtual ~response_handler_info() = default;
	void init() {
		init_get_robot_location_handler();
	}
	void init_get_robot_location_handler() {
		http_key key(GET, ZEG_ROBOT_LOCATION);
		http_utility::get().registe_handler(key, &get_robot_location_handler);
	}
private:
	get_robot_location_response_handler get_robot_location_handler;
};
}




#endif /* SRC_RESPONSE_HANDLER_INFO_HPP_ */
