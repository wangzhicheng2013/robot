#ifndef SRC_URL_MAPPER_HPP_
#define SRC_URL_MAPPER_HPP_
#include <string>
#include <unordered_map>
using namespace std;
enum  url_type {
	ZEG_ROBOT_LOCATION,
	UNKNOWN_ZEG_ROBOT_TYPE
};
class url_mapper {
public:
	inline static url_mapper &get() {
		static url_mapper mapper;
		return mapper;
	}
	url_type get_type(const string &url) {
		string tmp;
		auto pos = url.find('?');
		if (pos != string::npos) {
			tmp.assign(url, 0, pos);
		}
		else {
			tmp = url;
		}
		auto it = mapper_.find(tmp);
		if (end(mapper_) == it) {
			return UNKNOWN_ZEG_ROBOT_TYPE;
		}
		return it->second;
	}
private:
	url_mapper() {
		mapper_.insert(make_pair("/robot/location", ZEG_ROBOT_LOCATION));
	}
	virtual ~url_mapper() = default;
private:
	unordered_map<string, url_type>mapper_;			// key - url
};

#endif /* SRC_URL_MAPPER_HPP_ */
