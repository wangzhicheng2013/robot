#ifndef SRC_ZEG_ROBOT_NETWORK_MESSAGE_EXCEPTION_LOGGER_HPP_
#define SRC_ZEG_ROBOT_NETWORK_MESSAGE_EXCEPTION_LOGGER_HPP_
#include "zeg_robot_network_message_exception_handler.hpp"
#include "NanoLog.hpp"
namespace zeg_robot_scheduler_communication {
class zeg_robot_network_message_exception_logger : public zeg_robot_network_message_exception_handler {
public:
	bool handler(const zeg_robot_header &header) override {
		LOG_CRIT << header.robot_id << " network message latency over the water mark.";
		return true;
	}
};
}

#endif /* SRC_ZEG_ROBOT_NETWORK_MESSAGE_EXCEPTION_LOGGER_HPP_ */
