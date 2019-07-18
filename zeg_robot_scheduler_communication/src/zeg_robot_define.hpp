#ifndef SRC_ZEG_ROBOT_DEFINE_H_
#define SRC_ZEG_ROBOT_DEFINE_H_
#include <stdint.h>
#include <string>
#include <vector>
#include "msgpack.hpp"
namespace zeg_robot_scheduler_communication {
using namespace std;
struct zeg_robot_header {
	zeg_robot_header() {
		timestamp = 0;
	}
	zeg_robot_header(const string &a, const string &b, uint64_t c) :
		type(a),
		robot_id(b),
		timestamp(c) {

	}
	string type;
	string robot_id;
	uint64_t timestamp;
	MSGPACK_DEFINE(type, robot_id, timestamp);
};
struct zeg_robot_point {
	zeg_robot_point() {
		x = y = 0;
	}
	zeg_robot_point(double a, double b) :
		x(a),
		y(b) {
	}
	double x, y;
	MSGPACK_DEFINE(x, y);
};
struct zeg_robot_basic_info {
	zeg_robot_basic_info() {
		cur_theta = 0;
        state = 0;
        battery_percentage = 0;
    }
	zeg_robot_basic_info(const zeg_robot_point &a,
			zeg_robot_point &b,
			zeg_robot_point &c,
			double d,
			uint32_t e,
			double f) : last_point(a), next_point(b), cur_point(c), cur_theta(d), state(e), battery_percentage(f) {

	}
    zeg_robot_point last_point;
	zeg_robot_point next_point;
	zeg_robot_point cur_point;
	double cur_theta;
	uint32_t state;
	double battery_percentage;
	MSGPACK_DEFINE(last_point, next_point, cur_point, cur_theta, state, battery_percentage);
};
struct zeg_robot_task {
	string task_id;
	uint32_t task_type;
	vector<zeg_robot_point>points;
	double last_point_theta;
	MSGPACK_DEFINE(task_id, task_type, points, last_point_theta);
};
struct zeg_robot_task_ack {
	string task_id;
	MSGPACK_DEFINE(task_id);
};
struct zeg_robot_task_finish {
	string task_id;
	uint32_t task_state;
	MSGPACK_DEFINE(task_id, task_state);
};
struct zeg_robot_task_finish_ack {
	string task_id;
	MSGPACK_DEFINE(task_id);
};
struct zeg_robot_point_lock {
	string task_id;
	vector<zeg_robot_point>locked_points;
	MSGPACK_DEFINE(task_id, locked_points);
};
struct zeg_robot_point_lock_ack {
	string task_id;
	vector<zeg_robot_point>locked_points;
	MSGPACK_DEFINE(task_id, locked_points);
};
struct zeg_robot_point_free {
	string task_id;
	vector<zeg_robot_point>freed_points;
	MSGPACK_DEFINE(task_id, freed_points);
};
struct zeg_robot_point_free_ack {
	string task_id;
	MSGPACK_DEFINE(task_id);
};
struct zeg_robot_command_unpack_struct {
	zeg_robot_command_unpack_struct(msgpack::unpacked *a,
			const char *b,
			const zeg_robot_header *c,
			int d,
			size_t e) : unpack_msg(a),
					unpack_buf(b),
					unpack_header(c),
					unpack_len(d),
					unpack_offset(e) {

	}
	msgpack::unpacked *unpack_msg;
	const char *unpack_buf;
	const zeg_robot_header *unpack_header;
	int unpack_len;
	size_t unpack_offset;
};
}

#endif /* SRC_ZEG_ROBOT_DEFINE_H_ */
