#ifndef SRC_ZEG_COMMAND_PARSER_HPP_
#define SRC_ZEG_COMMAND_PARSER_HPP_
#include "zeg_robot_define.hpp"
#include "zeg_config.hpp"
#include "rpc_client.hpp"
#include "codec.h"
namespace zeg_message_interface {
using namespace rest_rpc;
using namespace rest_rpc::rpc_service;
class zeg_command_parser {
public:
	zeg_command_parser() = default;
	virtual ~zeg_command_parser() = default;
public:
	virtual bool parse(zeg_command_unpack_struct *unpack_cmd, string &ack_str) = 0;
};
}

#endif /* SRC_ZEG_COMMAND_PARSER_HPP_ */
