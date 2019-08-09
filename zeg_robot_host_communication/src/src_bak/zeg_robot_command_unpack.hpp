#ifndef SRC_ZEG_ROBOT_COMMAND_UNPACK_HPP_
#define SRC_ZEG_ROBOT_COMMAND_UNPACK_HPP_
#include "zeg_robot_define.hpp"
namespace zeg_robot_host_communication {
template<class cmd_type>
class zeg_robot_command_unpack {
public:
	static inline zeg_robot_command_unpack &get_instance() {
		static zeg_robot_command_unpack obj;
		return obj;
	}
	bool unpack(const string &cmd_str, zeg_robot_header &header, cmd_type &cmd) {
		size_t offset = 0;
		msgpack::unpacked msg;
		msgpack::unpack(&msg, cmd_str.c_str(), cmd_str.size(), &offset);
		msgpack::object obj = msg.get();
		bool no_excepption = true;
		try {
			obj.convert(&header);
			msgpack::unpack(&msg, cmd_str.c_str(), cmd_str.size(), &offset);
			obj = msg.get();
			obj.convert(&cmd);
		}
		catch (...) {
			no_excepption = false;
		}
		return no_excepption;
	}
private:
	zeg_robot_command_unpack() = default;
	zeg_robot_command_unpack(const zeg_robot_command_unpack &) = delete;
	zeg_robot_command_unpack & operator = (const zeg_robot_command_unpack &) = delete;
	~zeg_robot_command_unpack() = default;
};
}

#endif /* SRC_ZEG_ROBOT_COMMAND_PACK_HPP_ */
