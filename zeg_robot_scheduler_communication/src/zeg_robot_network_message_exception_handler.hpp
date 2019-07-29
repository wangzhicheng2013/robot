#ifndef SRC_ZEG_ROBOT_NETWORK_MESSAGE_EXCEPTION_HANDLER_HPP_
#define SRC_ZEG_ROBOT_NETWORK_MESSAGE_EXCEPTION_HANDLER_HPP_
#include "zeg_robot_define.hpp"
namespace zeg_robot_scheduler_communication {
class zeg_robot_network_message_exception_handler {
public:
	zeg_robot_network_message_exception_handler() = default;
	virtual ~zeg_robot_network_message_exception_handler() = default;
public:
	virtual bool handler(const zeg_robot_header &) = 0;
};
}

#endif /* SRC_ZEG_ROBOT_NETWORK_MESSAGE_EXCEPTION_HANDLER_HPP_ */
