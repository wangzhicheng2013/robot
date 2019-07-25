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
REFLECTION(zeg_robot_get_location, id, pointId);

struct zeg_robot_set_locations {
	string id;
	vector<string>pointIds;
};
REFLECTION(zeg_robot_set_locations, id, pointIds);

struct zeg_robot_task_assignment {
	string id;
	string type;
};
REFLECTION(zeg_robot_task_assignment, id, type);

struct zeg_robot_task_assignment_result {
	string id;
	bool accepted;
	string reason;
};
REFLECTION(zeg_robot_task_assignment_result, id, accepted, reason);

struct zeg_robot_task_termination {
	string id;
	string status;
};
REFLECTION(zeg_robot_task_termination, id, status);
}



#endif /* SRC_ZEG_ROBOT_REST_STRUCT_HPP_ */
