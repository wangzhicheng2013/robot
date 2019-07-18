/*****************************************************************************
*  zeg stat output                                                           *
*  Copyright (C) 2019                                                        *
*                                                                            *
*  @file     zeg_stat_output.hpp                                             *
*  @brief    zeg stat output to log                                          *
*  Details.                                                                  *
*                                                                            *
*  @author                                                                   *
*  @email                                                                    *
*  @version  1.0.0                                                           *
*  @date     2019-05-29                                                      *
*  @license                                                                  *
*                                                                            *
*----------------------------------------------------------------------------*
*  Change History :                                                          *
*  <Date>     | <Version> | <Author>       | <Description>                   *
*----------------------------------------------------------------------------*
*  2019/05/29 | 1.0.0     |                | Create file                     *
*----------------------------------------------------------------------------*                                                                   *
*****************************************************************************/
#ifndef SRC_ZEG_STAT_OUTPUT_HPP_
#define SRC_ZEG_STAT_OUTPUT_HPP_
#include "zeg_config.hpp"
#include "base_thread.hpp"
namespace zeg_message_interface {
class zeg_stat_output : public base_thread {
public:
	zeg_stat_output() {
		init_navigate_value();
	}
protected:
	virtual void process() override {
		while (true) {
			this_thread::sleep_for(chrono::seconds(zeg_config::get_instance().stat_log_interval));
			stat_recv_command(NAVIGATE_COMMAND, navigate_interval_value, navigate_last_value);
		}
	}
private:
	inline void init_navigate_value() {
		navigate_interval_value = 0;
		navigate_last_value = 0;
	}
	void stat_recv_command(unsigned char cmd_type, uint64_t &interval_value, uint64_t &last_value) {
		uint64_t count = 0;
		const char *str0 = nullptr;
		const char *str1 = nullptr;
		switch (cmd_type) {
		case NAVIGATE_COMMAND:
			count = zeg_config::get_instance().recv_navigate_cmd_counter_.load(memory_order_acquire);
			str0 = "interval receive navigate command count = ";
			str1 = "total    receive navigate command count = ";
			break;
		case LOCATE_COMMAND:
			break;
		default:
			LOG_CRIT << "unknown command type.";
			return;
		}
		if (0 == last_value) {
			interval_value = count;
		}
		else {
			interval_value = count - last_value;
		}
		last_value = count;
		LOG_INFO << str0 << interval_value;
		LOG_INFO << str1 << count;
	}
private:
	uint64_t navigate_interval_value;
	uint64_t navigate_last_value;
public:
	void test_stat_recv_navigate_command() {
		uint64_t navigate_interval_value = 0;
		uint64_t navigate_last_value = 0;
		stat_recv_command(NAVIGATE_COMMAND, navigate_interval_value, navigate_last_value);
	}
};
}

#endif /* SRC_ZEG_STAT_OUTPUT_HPP_ */
