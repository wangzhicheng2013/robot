#ifndef SRC_ZEG_ROBOT_DEFINE_H_
#define SRC_ZEG_ROBOT_DEFINE_H_
#include <stdint.h>
#include <string>
#include <vector>
#include "msgpack.hpp"
namespace zeg_robot_navigation_escort {
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
	string type;				// zeg.robot
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
	bool operator == (const zeg_robot_point &point) {
		return (x == point.x) && (y == point.y);
	}
	friend bool operator == (const zeg_robot_point &point0, const zeg_robot_point &point1) {
		return (point0.x == point1.x) && (point0.y == point1.y);
	}
	double x, y;
	MSGPACK_DEFINE(x, y);
};
struct zeg_robot_navigation_command {
	string task_id;
	vector<zeg_robot_point>points;
	MSGPACK_DEFINE(task_id, points);
};
struct zeg_robot_navigation_command_ack {
	string task_id;
	MSGPACK_DEFINE(task_id);
};
struct zeg_robot_navigation_lock_point {
	void clear() {
		task_id.clear();
		locked_points.clear();
	}
	string task_id;
	vector<zeg_robot_point>locked_points;
	MSGPACK_DEFINE(task_id, locked_points);
};
struct zeg_robot_navigation_lock_point_ack {
	string task_id;
	vector<zeg_robot_point>locked_points;
	MSGPACK_DEFINE(task_id, locked_points);
};
struct zeg_robot_pose {
	inline zeg_robot_pose(double x = 0, double y = 0, double theta = 0) : x(x), y(y), theta(theta) {
	}
	zeg_robot_pose(const zeg_robot_pose &other) {
		x = other.x;
		y = other.y;
		theta = other.theta;
	}
	zeg_robot_pose & operator = (const zeg_robot_pose &other) {
		x = other.x;
		y = other.y;
		theta = other.theta;
		return *this;
	}
	bool operator == (const zeg_robot_pose &other) {
		return (x == other.x) && (y == other.y) && (theta == other.theta);
	}
	friend bool operator == (const zeg_robot_pose &one, const zeg_robot_pose &other) {
		return (one.x == other.x) && (one.y == other.y) && (one.theta == other.theta);
	}
	inline void normalize() {
		theta = atan2(sin(theta), cos(theta));
    }
	inline void set_zero() {
		x = 0;
		y = 0;
		theta = 0;
	}
    double x, y;
    double theta;
	MSGPACK_DEFINE(x, y, theta);
};
}

#endif /* SRC_ZEG_ROBOT_DEFINE_H_ */
