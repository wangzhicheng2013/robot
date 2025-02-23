#ifndef SRC_ZEG_ROBOT_DEFINE_HPP_
#define SRC_ZEG_ROBOT_DEFINE_HPP_
#include <stdint.h>
#include <cmath>
#include <vector>
#include "msgpack.hpp"
#include "common_utility.hpp"
namespace zeg_robot_simulator {
using namespace std;
static const double M_EPS = 0.00000001;
static const double M_PI_ = 3.1415926535898;
static const double DEG2RAD_ZEG = 0.017453292519943;
static const double RAD2DEG_ZEG = 57.295779513082;
struct robot_pose {
	inline robot_pose(double x = 0, double y = 0, double theta = 0) : x(x), y(y), theta(theta) {
	}
	robot_pose(const robot_pose &other) {
		x = other.x;
		y = other.y;
		theta = other.theta;
	}
	robot_pose & operator = (const robot_pose &other) {
		x = other.x;
		y = other.y;
		theta = other.theta;
		return *this;
	}
	bool operator == (const robot_pose &other) {
		return (x == other.x) && (y == other.y) && (theta == other.theta);
	}
	friend bool operator == (const robot_pose &one, const robot_pose &other) {
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
struct robot_speed {
	robot_speed(double vx = 0, double vy = 0, double w = 0) : vx(vx), vy(vy), w(w) {
	}
	robot_speed(const robot_speed &other) {
		vx = other.vx;
		vy = other.vy;
		w = other.w;
	}
	robot_speed & operator = (const robot_speed &other) {
		vx = other.vx;
		vy = other.vy;
		w = other.w;
		return *this;
	}
	bool operator == (const robot_speed &other) {
		return (vx == other.vx) && (vy == other.vy) && (w == other.w);
	}
	friend bool operator == (const robot_speed &one, const robot_speed &other) {
		return (one.vx == other.vx) && (one.vy == other.vy) && (one.w == other.w);
	}
	double vx, vy, w;
	MSGPACK_DEFINE(vx, vy, w);
};
template <typename T>
inline bool equal(T x, T y) {
	return fabs(x - y) < M_EPS;
}
inline double deg2rad(double angle) {
	return angle * DEG2RAD_ZEG;
}
inline double rad2deg(double angle) {
    return angle * RAD2DEG_ZEG;
}
template <typename T>
inline int sign(T d) {
	return (d >= 0) ? 1 : -1;
}
inline void add(const robot_pose &p0, const robot_pose &p1, robot_pose &p) {
	p.x = p0.x + p1.x;
	p.y = p0.y + p1.y;
	p.theta = p0.theta + p1.theta;
}
inline double direction(const robot_pose &src_pose, const robot_pose &target_pose) {
	return atan2(target_pose.y - src_pose.y, target_pose.x - src_pose.x);
}
double normalize(double angle) {
	if (fabs(angle) > M_PI_ * 2) {
		angle = angle - long(angle / M_PI_ * 2) * M_PI_ * 2;
	}
	while (angle > M_PI_) {
		angle -= M_PI * 2;
	}
	while (angle <= -M_PI_) {
		angle += M_PI_ * 2;
	}
	return angle;
}
void absolute_sum(const robot_pose &p0, const robot_pose &p1, robot_pose &p) {
	double s = sin(p0.theta);
	double c = cos(p0.theta);
	p.x = c * p1.x - s * p1.y;
	p.y = s * p1.x + c * p1.y;
	p.theta = p1.theta;
	add(p, p0, p);
    p.normalize();
}
inline double distance(const robot_pose &a, const robot_pose &b) {
	return hypot(a.x - b.x, a.y - b.y);
}
}

#endif /* SRC_ZEG_ROBOT_DEFINE_HPP_ */
