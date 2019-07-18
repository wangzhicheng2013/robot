#ifndef SRC_ZEG_ROBOT_COMMAND_PACK_HPP_
#define SRC_ZEG_ROBOT_COMMAND_PACK_HPP_
#include "zeg_robot_define.hpp"
namespace zeg_robot_host_communication {
template<class cmd_type>
class zeg_robot_command_pack {
public:
	static inline zeg_robot_command_pack &get_instance() {
		static zeg_robot_command_pack obj;
		return obj;
	}
	void pack(const zeg_robot_header &header, const cmd_type &cmd, string &str) {
		msgpack::sbuffer buffer_header;
		msgpack::sbuffer buffer_body;
		msgpack::pack(buffer_header, header);
		msgpack::pack(buffer_body, cmd);
		str.assign(buffer_header.data(), buffer_header.size());
		str.append(buffer_body.data(), buffer_body.size());
	}
private:
	zeg_robot_command_pack() = default;
	zeg_robot_command_pack(const zeg_robot_command_pack &) = delete;
	zeg_robot_command_pack & operator = (const zeg_robot_command_pack &) = delete;
	~zeg_robot_command_pack() = default;
};
}

#endif /* SRC_ZEG_ROBOT_COMMAND_PACK_HPP_ */
