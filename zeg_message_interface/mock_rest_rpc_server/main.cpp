/*****************************************************************************
*  zeg mock_navigate_server                                                  *
*  Copyright (C) 2019                                                        *
*                                                                            *
*  @file    main.cpp                                                         *
*  @brief   zeg mock navigate command server                                 *
*  Details.                                                                  *
*                                                                            *
*  @author                                                                   *
*  @email                                                                    *
*  @version  1.0.0                                                           *
*  @date     2019-05-30                                                      *
*  @license                                                                  *
*                                                                            *
*----------------------------------------------------------------------------*
*  Change History :                                                          *
*  <Date>     | <Version> | <Author>       | <Description>                   *
*----------------------------------------------------------------------------*
*  2019/05/30 | 1.0.0     |                | Create file                     *
*----------------------------------------------------------------------------*                                                                   *
*****************************************************************************/
#include "zeg_data_define.h"
#include "rpc_server.h"
using namespace rest_rpc;
using namespace rpc_service;
using namespace zeg_message_interface;

class zeg_mock_navigate_server {
public:
	uint64_t get_taskid(rpc_conn conn, const znavigate_command &cmd) {
		return cmd.task_id;
	}
};
int main() {
	zeg_mock_navigate_server obj;
	rpc_server server(RPC_SERVER_PORT, thread::hardware_concurrency());
	server.register_handler("get_taskid", &zeg_mock_navigate_server::get_taskid, &obj);
	server.run();

	return 0;
}
