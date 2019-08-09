#ifndef SRC_MESSAGE_COMMUNICATE_ENTITY_MAKER_HPP_
#define SRC_MESSAGE_COMMUNICATE_ENTITY_MAKER_HPP_
#include "message_communicate_entity.hpp"
#include <string>
#include <unordered_map>
#include <functional>
#include <memory>
class message_communicate_entity_maker {
public:
	static message_communicate_entity *make(const string &entity_type) {
		auto it = map_.find(entity_type);
		if (end(map_) == it) {
			return nullptr;
		}
		return it->second();
	}
	static unique_ptr<message_communicate_entity>make_unique_ptr(const string &entity_type) {
		return unique_ptr<message_communicate_entity>(make(entity_type));
	}
	static std::shared_ptr<message_communicate_entity>make_shared_ptr(const string &entity_type) {
		return std::shared_ptr<message_communicate_entity>(make(entity_type));
	}
public:
	template<class T>
	struct register_t {
		register_t(const string &key){
			message_communicate_entity_maker::get().map_.emplace(key, [] {return new T();});
		}
		template<typename... Args>
		register_t(const string &key, Args... args){
			message_communicate_entity_maker::get().map_.emplace(key, [=] {return new T(args...);});
		}
	};
private:
	message_communicate_entity_maker() = default;
	message_communicate_entity_maker(const message_communicate_entity_maker &) = delete;
	message_communicate_entity_maker operator = (const message_communicate_entity_maker &) = delete;
	message_communicate_entity_maker(message_communicate_entity_maker &&) = delete;
	~message_communicate_entity_maker() = default;
private:
	inline static message_communicate_entity_maker &get() {
		static message_communicate_entity_maker maker;
		return maker;
	}
private:
	static unordered_map<string, function<message_communicate_entity *()>>map_;	// key -- entity type such udp_unicast
};
unordered_map<string, function<message_communicate_entity *()>>message_communicate_entity_maker::map_;
#define REGISTER_MESSAGE_COMMUNICATE_ENTITY_VNAME(T) reg_entity_##T##_
#define REGISTER_MESSAGE_COMMUNICATE_ENTITY(T, key, ...) static message_communicate_entity_maker::register_t<T> REGISTER_MESSAGE_COMMUNICATE_ENTITY_VNAME(T)(key, ##__VA_ARGS__);

#endif /* SRC_MESSAGE_COMMUNICATE_ENTITY_MAKER_HPP_ */
