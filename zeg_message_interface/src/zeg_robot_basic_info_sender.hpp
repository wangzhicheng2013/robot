#ifndef SRC_ZEG_ROBOT_BASIC_INFO_SENDER_HPP_
#define SRC_ZEG_ROBOT_BASIC_INFO_SENDER_HPP_
#include "zeg_config.hpp"
namespace zeg_message_interface {
class zeg_robot_basic_info_sender {
public:
	static inline zeg_robot_basic_info_sender &get_instance() {
		static zeg_robot_basic_info_sender sender;
		return sender;
	}
	bool send(const zeg_robot_header &header, const zeg_robot_basic_info &body) {
		msgpack::sbuffer buffer_header;
		msgpack::sbuffer buffer_body;
		msgpack::pack(buffer_header, header);
		msgpack::pack(buffer_body, body);
		string str(buffer_header.data(), buffer_header.size());
		str.append(buffer_body.data(), buffer_body.size());
		LOG_INFO << "get pose from host = (" << body.cur_point.x << "," << body.cur_point.y << "," << body.cur_theta << ")";
		int size = zeg_config::get_instance().udp_broadcast_client_ptr_->send(str.c_str(), str.size());
		LOG_INFO << "send broadcast message size = " << size;
		return size == str.size();
	}
private:
	zeg_robot_basic_info_sender() = default;
	zeg_robot_basic_info_sender(const zeg_robot_basic_info_sender &) = delete;
	zeg_robot_basic_info_sender operator = (const zeg_robot_basic_info_sender &) = delete;
	~zeg_robot_basic_info_sender() = default;
};
}


#endif /* SRC_ROBOT_INFO_BASIC_LOADER_HPP_ */
