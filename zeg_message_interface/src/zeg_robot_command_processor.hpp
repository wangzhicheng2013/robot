#ifndef SRC_ZEG_ROBOT_COMMAND_PROCESSOR_HPP_
#define SRC_ZEG_ROBOT_COMMAND_PROCESSOR_HPP_
#include "zeg_robot_define.hpp"
#include "zeg_config.hpp"
#include "zeg_robot_task_parser.hpp"
#include "zeg_robot_point_lock_ack_parser.hpp"
#include "zeg_robot_point_free_ack_parser.hpp"
#include "zeg_robot_task_finish_ack_parser.hpp"
namespace zeg_message_interface {
class zeg_robot_command_processor {
public:
	inline static zeg_robot_command_processor &get_instance() {
		static zeg_robot_command_processor processor;
		return processor;
	}
private:
	zeg_robot_command_processor() = default;
	virtual ~zeg_robot_command_processor() = default;
public:
	bool process(const char *buf, int len, string &ack_str) {
		bool res = true;
		msgpack::unpacked msg;
		size_t offset = 0;
		msgpack::unpack(&msg, buf, len, &offset);
		msgpack::object obj = msg.get();
		zeg_robot_header header;
		try {
			obj.convert(&header);
			zeg_robot_command_unpack_struct unpack_struct(&msg, buf, &header, len, offset);
			auto parser = zeg_robot_command_parser_maker::make_unique_ptr(header.type);
			if (nullptr == parser) {
				LOG_CRIT << header.type << " not be registed...!";
				return false;
			}
			res = parser->parse(&unpack_struct, ack_str);
		}
		catch(const std::exception &e) {
			LOG_CRIT << e.what();
			return false;
		}
		if (header.robot_id != zeg_config::get_instance().robot_id) {
			res = false;
		}
		return res;
	}
};
}

#endif /* SRC_ZEG_ROBOT_COMMAND_PROCESSOR_HPP_ */
