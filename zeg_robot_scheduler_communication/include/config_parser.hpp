/*****************************************************************************
*  configuration file parser utility                                         *
*  Copyright (C) 2019                                                        *
*                                                                            *
*  @file     config_parser.hpp                                               *
*  @brief    configuration file parser utility                               *
*  Details.                                                                  *
*                                                                            *
*  @author                                                                   *
*  @email                                                                    *
*  @version  1.0.0                                                           *
*  @date     2019-06-02                                                      *
*  @license                                                                  *
*                                                                            *
*----------------------------------------------------------------------------*
*  Change History :                                                          *
*  <Date>     | <Version> | <Author>       | <Description>                   *
*----------------------------------------------------------------------------*
*  2019/06/02 | 1.0.0     |                | Create file                     *
*----------------------------------------------------------------------------*
*****************************************************************************/

#ifndef INCLUDE_CONFIG_PARSER_HPP_
#define INCLUDE_CONFIG_PARSER_HPP_
#include <string>
#include <vector>
#include <unordered_map>
#include <set>
#include <fstream>
#include <algorithm>
namespace config_parser {
using namespace std;
class config_parser;
class inner_key {
	friend class config_parser;
private:
	unordered_map<string, set<string> >map_inner_;	// key -- inner key value -- inner value
private:
	inline void insert(const string &key, const set<string>&value) {
		map_inner_.insert(make_pair(key, value));
	}
	inline void clear() {
		map_inner_.clear();
	}
	inline bool empty() {
		return map_inner_.empty();
	}
	void get_value(const string &key, vector<string>&value) {
		auto it = map_inner_.find(key);
		if (it == map_inner_.end()) {
			return;
		}
		set<string>&_value = it->second;
		value.clear();
		value.resize(_value.size());
		copy(begin(_value), end(_value), begin(value));
	}
};
class config_parser {
public:
	inline static config_parser &get_instance() {
		return config_parser_;
	}
private:
	config_parser(char comment = '#', char split0 = '=', char split1 = ',') :
		comment_char(comment), split_char_kv(split0), split_char_vv(split1) {
	}
	virtual ~config_parser() = default;
public:
	bool init(const char *filepath) {
		ifstream fstream;
		if (false == get_fstream(filepath, fstream)) {
			return false;
		}
		inner_key inner_key_obj;
		get_inner_keys(fstream, inner_key_obj);
		fstream.close();
		return !map_conf_.empty();
	}
	void get_value(const string &firstkey, const string &secondkey, vector<string>&values) {
		values.clear();
		auto it = map_conf_.find(firstkey);
		if (map_conf_.end() == it) {
			return;
		}
		inner_key &inner_key_obj = it->second;
		inner_key_obj.get_value(secondkey, values);
	}
public:
	static void split_to_set(const string &line, char split_char, set<string>&values) {
		string value;
		values.clear();
    	for (size_t i = 0;line[i];++i) {
    		if (split_char != line[i]) {
    			value += line[i];
    			continue;
    		}
    		values.insert(value);
    		value.clear();
    	}
    	if (!value.empty()) {
    		values.insert(value);
    	}
    }
private:
	inline bool get_fstream(const char *filepath, ifstream &fstream) {
		if (nullptr == filepath) {
			return false;
		}
		fstream.open(filepath, ios::in);
		if(false == fstream.is_open()) {
			fstream.close();
			return false;
		}
		return true;
	}
	inline bool skip_line(const string &line) {
		return line.empty() || ('#' == line[0]);
	}
	inline void insert_first_key(string &firstkey, string &lastfirstkey, inner_key &inner_key_obj) {
		if (lastfirstkey != firstkey) {
			map_conf_.insert(make_pair(lastfirstkey, inner_key_obj));
			lastfirstkey = firstkey;
			inner_key_obj.clear();
		}
	}
	void get_inner_keys(ifstream &fstream, inner_key &inner_key_obj) {
		string line, newline;
		string lastfirstkey, firstkey, secondkey;
		string::size_type size = 0, pos = 0;
		set<string>values;
		while (!fstream.eof()) {
			fstream >> line;
			if (true == skip_line(line)) {
				continue;
			}
			size = line.size();
			// get [key]
			if (('[' == line[0]) && (']' == line[size - 1])) {
				firstkey.assign(line, 1, size - 2);
				insert_first_key(firstkey, lastfirstkey, inner_key_obj);
				continue;
			}
			// get inner key and values
			if ((pos = line.find(split_char_kv)) != string::npos) {
				secondkey.assign(line, 0, pos);
				newline.assign(line, pos + 1, size);
				split_to_set(newline, split_char_vv, values);
				inner_key_obj.insert(secondkey, values);
			}
			line.clear();
		}
		if (!inner_key_obj.empty()) {
			map_conf_.insert(make_pair(firstkey, inner_key_obj));
		}
	}
public:
	static config_parser config_parser_;
private:
	char comment_char;								// comment line generally #
	char split_char_kv;								// split char between key and value generally =
	char split_char_vv;								// split char between value and value generally ,
	unordered_map<string, inner_key >map_conf_;		// key -- outside key
};
config_parser config_parser::config_parser_;
}
#endif /* INCLUDE_CONFIG_PARSER_HPP_ */
