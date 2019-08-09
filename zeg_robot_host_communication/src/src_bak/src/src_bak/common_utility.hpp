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
void make_uuid(string &uuid) {
	uuid = sole::uuid0().str();
}

#endif /* SRC_COMMON_UTILITY_HPP_ */
