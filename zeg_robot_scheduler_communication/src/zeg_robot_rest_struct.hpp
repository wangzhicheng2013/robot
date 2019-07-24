#ifndef SRC_ZEG_ROBOT_REST_STRUCT_HPP_
#define SRC_ZEG_ROBOT_REST_STRUCT_HPP_
#include <string>
#include <vector>
#include "json.hpp"
namespace zeg_robot_scheduler_communication {
using namespace std;
struct zeg_robot_get_location {
	string id;
	string pointId;
};
REFLECTION(zeg_robot_get_location, id, pointId)

struct zeg_robot_set_location {
	string id;
	vector<string>pointIds;
};
REFLECTION(zeg_robot_set_location, id, pointIds);
}



#endif /* SRC_ZEG_ROBOT_REST_STRUCT_HPP_ */
