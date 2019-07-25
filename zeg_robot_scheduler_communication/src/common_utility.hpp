#ifndef SRC_COMMON_UTILITY_HPP_
#define SRC_COMMON_UTILITY_HPP_
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include "sole.hpp"
#include "json.hpp"
using namespace std;
template <typename T>
void merge_vector(const vector<T>&v0, vector<T>&v1) {
	for_each(begin(v0), end(v0), [&v1](auto &e) {
		v1.emplace_back(e);
	});
}
inline void run_program(const char* path) {
	if (0 == std::system(path)) {
		cout << path << " start ok." << endl;
	}
	else {
		cout << path << " start failed." << endl;
	}
}
void kill_program(const char *program_name) {
	char cmd[128] = "";
	snprintf(cmd, sizeof(cmd), "ps x | grep ./%s | grep -v grep | awk \'{print $1}\'", program_name);
	FILE* fp = popen(cmd, "r");
	if (!fp) {
			cerr << cmd << " failed...!" << endl;
			return;
	}
	char buf[64] = "";
	while (fgets(buf, sizeof(buf), fp)) {
	}
	pclose(fp);
	snprintf(cmd, sizeof(cmd), "kill -9  %s", buf);
	cout << cmd << endl;
	if (0 == std::system(cmd)) {
		cout << program_name << " end ok." << endl;
	}
}
inline void make_uuid(string &uuid) {
	uuid = sole::uuid0().str();
}
void get_url_key_val(const char *uri, vector<string>&keys, vector<string>&values) {
	const char *p = uri;
	while (*p) {
		if('?' == *p) {
			break;
		}
		p++;
	}
	if(0 == *p) {
		return;
	}
	char tmp[1024] = "";
	int loop = 0;
	bool Get = false;
	while (*p) {
		if (*(p + 1) && !Get) {
			sscanf(p + 1, "%[^= | &]", tmp);
			if (strcmp(tmp, "")) {
				Get = true;
				if (!loop) {
					keys.emplace_back(tmp);
				}
				else {
					values.emplace_back(tmp);
				}
			}
		}
		p++;
		if (0 == *p) {
			break;
		}
		if (('=' == *p) || ('&' == *p)) {
			if ('=' == *p) {
				loop = 1;
			}
			else {
				loop = 0;
			}
			Get = false;
		}
	}
}
template <typename T>
inline void serialize_to_json(const T &obj, string &json) {
	iguana::string_stream ss;
	iguana::json::to_json(ss, obj);
	json = ss.str();
}
template <typename T>
inline bool deserialize_from_json(const string &json, T &obj) {
	return iguana::json::from_json(obj, json.data(), json.length());
}

#endif /* SRC_COMMON_UTILITY_HPP_ */
