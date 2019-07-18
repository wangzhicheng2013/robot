#ifndef SRC_ZEG_RECV_COMMAND_HPP_
#define SRC_ZEG_RECV_COMMAND_HPP_
#include "zeg_config.hpp"
#include "base_thread.hpp"
#include "rpc_client.hpp"
#include "codec.h"
namespace zeg_message_interface {
using namespace rest_rpc;
using namespace rest_rpc::rpc_service;
class zeg_recv_command : public base_thread {
public:
	zeg_recv_command() : rpc_client_ptr_(new rpc_client) {
		rpc_connected_ = false;
	}
	bool init() {
		if (nullptr == rpc_client_ptr_) {
			return false;
		}
		rpc_connected_ = rpc_client_ptr_->connect(zeg_config::zeg_config::get_instance().RPC_SERVER_IP, zeg_config::get_instance().robot_rpc_message_interface_layer_port, 5);
		return true;
	}
    bool call_command_process_rpc(const char *buf, int len, string &ack_str) {
        bool no_exception = true;
        bool res = true;
        if (false == rpc_connected_) {
            rpc_connected_ = rpc_client_ptr_->connect(zeg_config::get_instance().RPC_SERVER_IP, zeg_config::get_instance().robot_rpc_message_interface_layer_port, 5);
        }
        try {
        	string str(buf, len);
        	ack_str = rpc_client_ptr_->call<string>("process_command", str);
        }
        catch (const std::exception& e) {
            LOG_CRIT << e.what();
            no_exception = false;
        }
        return no_exception && (false == ack_str.empty());
    }
protected:
	virtual void process() override {
		char recv_buf[zeg_config::get_instance().buffer_size] = "";
		int recv_len = 0;
		int len = 0;
		string ack_str;
		while (true) {
			recv_len = zeg_config::get_instance().udp_unicast_server_ptr_->recv(recv_buf, sizeof(recv_buf));
			if (recv_len <= 0) {
				continue;
			}
			LOG_INFO << "receive scheduler command now.";
			if (true == call_command_process_rpc(recv_buf, recv_len, ack_str)) {
				len = zeg_config::get_instance().udp_unicast_server_ptr_->send(ack_str.c_str(), ack_str.size());
				LOG_INFO << "send ack = " << ack_str << " sent size = " << len;
			}
		}
	}
private:
	unique_ptr<rpc_client>rpc_client_ptr_;
private:
	bool rpc_connected_;
};
}

#endif /* SRC_ZEG_RECV_COMMAND_HPP_ */
