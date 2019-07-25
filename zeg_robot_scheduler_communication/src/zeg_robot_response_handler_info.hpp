#ifndef SRC_ZEG_ROBOT_RESPONSE_HANDLER_INFO_HPP_
#define SRC_ZEG_ROBOT_RESPONSE_HANDLER_INFO_HPP_
#include "http_utility.hpp"
#include "zeg_robot_location_query_response_handler.hpp"
#include "zeg_robot_locations_set_response_handler.hpp"
#include "zeg_robot_task_assignment_response_handler.hpp"
#include "zeg_robot_task_termination_response_handler.hpp"
namespace zeg_robot_scheduler_communication {
class zeg_robot_response_handler_info {
public:
	static inline zeg_robot_response_handler_info &get() {
		static zeg_robot_response_handler_info obj;
		return obj;
	}
	void init() {
		init_get_robot_location_handler();
		init_set_robot_locations_handler();
		init_task_assignment_response_handler();
		init_task_termination_response_handler();
	}
private:
	zeg_robot_response_handler_info() = default;
	virtual ~zeg_robot_response_handler_info() = default;
private:
	void init_get_robot_location_handler() {
		http_key key(GET, ZEG_ROBOT_LOCATION);
		http_utility::get().registe_handler(key, &robot_location_query_handler);
	}
	void init_set_robot_locations_handler() {
		http_key key(POST, ZEG_ROBOT_LOCATIONS);
		http_utility::get().registe_handler(key, &robot_locations_set_handler);
	}
	void init_task_assignment_response_handler() {
		http_key key(POST, ZEG_ROBOT_TASK_ASSIGNMENT);
		http_utility::get().registe_handler(key, &robot_task_assignment_response_handler);
	}
	void init_task_termination_response_handler() {
		http_key key(POST, ZEG_ROBOT_TASK_TERMINATION);
		http_utility::get().registe_handler(key, &robot_task_termination_response_handler);
	}
private:
	zeg_robot_location_query_response_handler robot_location_query_handler;
	zeg_robot_locations_set_response_handler robot_locations_set_handler;
	zeg_robot_task_assignment_response_handler robot_task_assignment_response_handler;
	zeg_robot_task_termination_response_handler robot_task_termination_response_handler;
};
}

#endif /* SRC_ZEG_ROBOT_RESPONSE_HANDLER_INFO_HPP_ */
