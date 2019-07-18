#ifndef SRC_ZEG_ROBOT_COMMAND_PARSER_HPP_
#define SRC_ZEG_ROBOT_COMMAND_PARSER_HPP_
#include "zeg_robot_define.hpp"
#include "rpc_client.hpp"
#include "codec.h"
#include "zeg_robot_config.hpp"
namespace zeg_robot_scheduler_communication {
using namespace rest_rpc;
using namespace rest_rpc::rpc_service;
struct zeg_robot_process_arg {
	explicit zeg_robot_process_arg(char *a, int b, void *c) : buf(a), len(b), client_addr(c) {
	}
	char *buf;
	int len;
	void *client_addr;
	string ack_str;
};
class zeg_robot_command_parser {
public:
	zeg_robot_command_parser() = default;
	virtual ~zeg_robot_command_parser() = default;
public:
	virtual bool parse(zeg_robot_command_unpack_struct *unpack_cmd, zeg_robot_process_arg &arg) = 0;
};
}

#endif /* SRC_ZEG_ROBOT_COMMAND_PARSER_HPP_ */
