#ifndef SRC_ZEG_ROBOT_RESEND_TASK_THREAD_HPP_
#define SRC_ZEG_ROBOT_RESEND_TASK_THREAD_HPP_
#include "base_thread.hpp"
#include "zeg_robot_task_escort.hpp"
#include "zeg_robot_udp_server.hpp"
#include "zeg_robot_command_unpack.hpp"
namespace zeg_robot_scheduler_communication {
class zeg_robot_resend_task_thread : public base_thread {
public:
	void set_udp_ptr(zeg_robot_udp_server *p) {
		udp_server_ptr_ = p;
	}
protected:
	virtual void process() override {
		int sleep_interval = zeg_robot_config::get_instance().robot_task_escort_sleep_interval;
		while (true) {
			zeg_robot_task_escort::get().get_taskid(taskid_);
			LOG_INFO << "get task id = " << taskid_ << " from queue.";
			this_thread::sleep_for(chrono::seconds(sleep_interval));
			if (false == zeg_robot_task_escort::get().get_task_pack_str(taskid_, pack_str_)) {
				LOG_INFO << "task id = " << taskid_ << " has been ack.";
				continue;
			}
			resend_task();
		}
	}
private:
	void resend_task() {
		int try_cnt = zeg_robot_config::get_instance().robot_task_escort_try_cnt;
		int sleep_interval = zeg_robot_config::get_instance().robot_task_escort_sleep_interval;
		string robot_id;
		if (false == get_robot_id(robot_id)) {
			return;
		}
		for (int i = 0;i < try_cnt;i++) {
			auto client_ptr = zeg_robot_update_address::get().get(robot_id);
			if (nullptr == client_ptr) {
				LOG_CRIT << "robot id = " << robot_id << " get client address failed.";
				return;
			}
			int size = udp_server_ptr_->send_to_client(pack_str_.c_str(), pack_str_.size(), *client_ptr);
			LOG_INFO << "send zeg robot task size = " << size;
			if (pack_str_.size() != size) {
				LOG_CRIT << "send size = " << size << " need send size = " << pack_str_.size();
			}
			this_thread::sleep_for(chrono::seconds(sleep_interval));
			if (false == zeg_robot_task_escort::get().get_task_pack_str(taskid_, pack_str_)) {
				LOG_INFO << "task id = " << taskid_ << " has been ack.";
				return;
			}
		}
		call_report_robot_exception(robot_id);
	}
	bool get_robot_id(string &robot_id) {
		zeg_robot_header header;
		zeg_robot_task robot_task;
		if (false == zeg_robot_command_unpack<zeg_robot_task>::get_instance().unpack(pack_str_, header, robot_task)) {
			LOG_CRIT << "unpack robot task failed.";
			return false;
		}
		robot_id = header.robot_id;
		return true;

	}
	bool call_report_robot_exception(const string &robot_id) {
		static rpc_client client(zeg_robot_config::zeg_robot_config::get_instance().RPC_SERVER_IP, zeg_robot_config::get_instance().robot_rpc_tcs_port);
		static bool connected = false;
		if (false == connected) {
			connected = client.connect(1);
		}
		bool succ = true;
		try {
			succ = client.call<bool>("report_robot_exception", robot_id);
		}
		catch (std::exception &e) {
			connected = false;
			succ = false;
			LOG_CRIT << e.what();
		}
		return succ;
	}
private:
	string taskid_;
	string pack_str_;
	zeg_robot_udp_server *udp_server_ptr_;

};
}

#endif /* SRC_ZEG_ROBOT_RESEND_TASK_THREAD_HPP_ */
