#ifndef SRC_ZEG_COMMAND_PARSER_MAKER_HPP_
#define SRC_ZEG_COMMAND_PARSER_MAKER_HPP_
#include <string>
#include <unordered_map>
#include <functional>
#include <memory>
#include "zeg_command_parser.hpp"
namespace zeg_message_interface {
class zeg_command_parser_maker {
public:
	static zeg_command_parser *make(const string &parser_type) {
		auto it = map_.find(parser_type);
		if (end(map_) == it) {
			return nullptr;
		}
		return it->second();
	}
	static unique_ptr<zeg_command_parser>make_unique_ptr(const string &parser_type) {
		return unique_ptr<zeg_command_parser>(make(parser_type));
	}
	static std::shared_ptr<zeg_command_parser>make_shared_ptr(const string &parser_type) {
		return std::shared_ptr<zeg_command_parser>(make(parser_type));
	}
public:
	template<class T>
	struct register_t {
		register_t(const string &key){
			zeg_command_parser_maker::get().map_.emplace(key, [] {return new T();});
		}
		template<typename... Args>
		register_t(const string &key, Args... args){
			zeg_command_parser_maker::get().map_.emplace(key, [=] {return new T(args...);});
		}
	};
private:
	zeg_command_parser_maker() = default;
	zeg_command_parser_maker(const zeg_command_parser_maker &) = delete;
	zeg_command_parser_maker operator = (const zeg_command_parser_maker &) = delete;
	zeg_command_parser_maker(zeg_command_parser_maker &&) = delete;
	~zeg_command_parser_maker() = default;
private:
	inline static zeg_command_parser_maker &get() {
		static zeg_command_parser_maker maker;
		return maker;
	}
private:
	static unordered_map<string, function<zeg_command_parser *()>>map_;	// key -- parser type such zeg_command_parser
};
unordered_map<string, function<zeg_command_parser *()>>zeg_command_parser_maker::map_;

#define REGISTER_COMMAND_PARSER_ENTITY_VNAME(T) reg_entity_##T##_
#define REGISTER_COMMAND_PARSER_ENTITY(T, key, ...) static zeg_command_parser_maker::register_t<T> REGISTER_COMMAND_PARSER_ENTITY_VNAME(T)(key, ##__VA_ARGS__);

}


#endif /* SRC_ZEG_COMMAND_PARSER_MAKER_HPP_ */
