#ifndef SRC_ZEG_ROBOT_NAVIGATION_LOCK_POINT_SENDER_HPP_
#define SRC_ZEG_ROBOT_NAVIGATION_LOCK_POINT_SENDER_HPP_
#include "zeg_config.hpp"
#include "zeg_robot_define.hpp"
namespace zeg_message_interface {
class zeg_robot_navigation_lock_point_sender {
public:
	static inline zeg_robot_navigation_lock_point_sender &get_instance() {
		static zeg_robot_navigation_lock_point_sender sender;
		return sender;
	}
	bool send(const zeg_robot_header &header, const zeg_robot_navigation_lock_point &body, unique_ptr<message_communicate_entity>&udp_sever_ptr) {
		msgpack::sbuffer buffer_header;
		msgpack::sbuffer buffer_body;
		msgpack::pack(buffer_header, header);
		msgpack::pack(buffer_body, body);
		string send_str(buffer_header.data(), buffer_header.size());
		send_str.append(buffer_body.data(), buffer_body.size());
		int size = udp_sever_ptr->send(send_str.c_str(), send_str.size());
		if (size > 1024) {
			LOG_CRIT << "send size over 1K.";
		}
		LOG_INFO << "send lock point size = " << size;
		if (send_str.size() != size) {
			LOG_CRIT << "send size = " << size << " need send size = " << send_str.size();
			return false;
		}
		return true;
	}
private:
	zeg_robot_navigation_lock_point_sender() = default;
	~zeg_robot_navigation_lock_point_sender() = default;
};
}




#endif /* SRC_ZEG_ROBOT_NAVIGATION_LOCK_POINT_SENDER_HPP_ */
