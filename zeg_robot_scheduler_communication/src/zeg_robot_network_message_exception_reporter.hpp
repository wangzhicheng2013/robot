#ifndef SRC_ZEG_ROBOT_NETWORK_MESSAGE_EXCEPTION_REPORTER_HPP_
#define SRC_ZEG_ROBOT_NETWORK_MESSAGE_EXCEPTION_REPORTER_HPP_
#include <functional>
#include <chrono>
#include "zeg_robot_network_message_exception_logger.hpp"
#include "common_events.hpp"
namespace zeg_robot_scheduler_communication {
class zeg_robot_network_message_exception_reporter {
public:
	using handler_delegate = std::function<bool(const zeg_robot_header &)>;
	using excception_events = common_events<handler_delegate>;
public:
	inline static zeg_robot_network_message_exception_reporter &get() {
		static zeg_robot_network_message_exception_reporter reporter;
		return reporter;
	}
	void set_latency_water_mark(unsigned m) {
		latency_water_mark = m;
	}
	bool report_exception(const zeg_robot_header &header) {
		if (chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count() - header.timestamp >= latency_water_mark) {
			return events_(header);
		}
		return false;
	}
private:
	zeg_robot_network_message_exception_reporter() {
		latency_water_mark = 100;			// 100ms
		events_ += std::bind(&zeg_robot_network_message_exception_logger::handler, &logger, std::placeholders::_1);
	}
	~zeg_robot_network_message_exception_reporter() {
		events_.clear();
	}
private:
	unsigned latency_water_mark;
	excception_events events_;
	zeg_robot_network_message_exception_logger logger;
};
}

#endif /* SRC_ZEG_ROBOT_NETWORK_MESSAGE_EXCEPTION_REPORTER_HPP_ */
