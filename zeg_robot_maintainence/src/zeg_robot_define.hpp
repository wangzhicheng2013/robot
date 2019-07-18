#ifndef SRC_ZEG_ROBOT_DEFINE_HPP_
#define SRC_ZEG_ROBOT_DEFINE_HPP_
#include <stdint.h>
#include <string>
#include "msgpack.hpp"
namespace zeg_robot_maintainence {
using namespace std;
struct robot_basic_info {
	robot_basic_info() {
		state = 0;
		x = y = theta = 0;
		battery_percentage = 0;
	}
	robot_basic_info(uint32_t a, double b, double c, double d, double e) :
		state(a),
		x(b),
		y(c),
		theta(d),
		battery_percentage(e) {

	}
	uint32_t state;
	double x, y, theta;
	double battery_percentage;
	MSGPACK_DEFINE(state, x, y, theta, battery_percentage);
};
struct zeg_robot_header {
	zeg_robot_header() {
		timestamp = 0;
	}
	zeg_robot_header(const string &a, const string &b, uint64_t c) :
		type(a),
		robot_id(b),
		timestamp(c) {

	}
	string type;				// zeg_robot
	string robot_id;
	uint64_t timestamp;
	MSGPACK_DEFINE(type, robot_id, timestamp);
};
}

#endif /* SRC_ZEG_ROBOT_DEFINE_HPP_ */
