/*****************************************************************************
*  zeg common data define                                                    *
*  Copyright (C) 2019                                                        *
*                                                                            *
*  @file     zeg_data_define.h                                               *
*  @brief    zeg common data structure define                                *
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
*  2019/05/27 | 1.0.0     |                | Create file                     *
*----------------------------------------------------------------------------*
*  2019/05/27 | 1.0.0     |                | Add command type                *
*----------------------------------------------------------------------------*
*  2019/05/30 | 1.0.0     |                | Add server port                 *                                                                   *
*****************************************************************************/
#ifndef SRC_ZEG_DATA_DEFINE_H_
#define SRC_ZEG_DATA_DEFINE_H_
#include <stdint.h>
#include <vector>
#include "msgpack.hpp"
namespace zeg_message_interface {
using namespace std;
enum {
	NAVIGATE_COMMAND,
	LOCATE_COMMAND
};
static const int RPC_SERVER_PORT = 9000;
static const char *RPC_SERVER_IP = "127.0.0.1";
typedef struct zpos {
    int x;
    int y;
    int theta;
    int blief;
	MSGPACK_DEFINE(x, y, theta, blief);
}zpos;
typedef struct zposetime {
    zpos pose_;
    uint64_t time;
    MSGPACK_DEFINE(pose_, time);
}zposetime;
typedef struct ztolerance {
	int x_thres;
	int y_thres;
	int theta_thres;
	int dist_thres;
	int time_thres;
	MSGPACK_DEFINE(x_thres, y_thres, theta_thres, dist_thres, time_thres);
}ztolerance;
typedef struct zwaypoint {
	zposetime posetime_;
	ztolerance tolerance_;
	MSGPACK_DEFINE(posetime_, tolerance_);
}zwaypoint;
typedef struct znavigate_command {
	uint64_t task_id;
	vector<zwaypoint>waypoints_;
	MSGPACK_DEFINE(task_id, waypoints_);
}znavigate_command;
}

#endif /* SRC_ZEG_DATA_DEFINE_H_ */
