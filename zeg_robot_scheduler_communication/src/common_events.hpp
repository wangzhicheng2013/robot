#ifndef SRC_COMMON_EVENTS_HPP_
#define SRC_COMMON_EVENTS_HPP_
#include <map>
template <class cb_func_type>
class common_events {
public:
	common_events() {
		next_key_ = 0;
	}
	common_events(const common_events &) = delete;
	common_events & operator = (const common_events &) = delete;
	virtual ~common_events() {
	}
private:
	inline void push_connection(int index, cb_func_type &cb_fun) {
		connections_.emplace(index, cb_fun);
	}
	inline void push_connection(int index, cb_func_type &&cb_fun) {
		connections_.emplace(index, move(cb_fun));
	}
	inline int assgin_connection(cb_func_type &&cb_fun) {
		int index = next_key_++;
		push_connection(index, forward<cb_func_type>(cb_fun));
		return index;
	}
	inline int connect(cb_func_type &cb_fun) {
        return assgin_connection(cb_fun);
    }
	inline int connect(cb_func_type &&cb_fun) {
        return assgin_connection(forward<cb_func_type>(cb_fun));
    }
	inline void disconnect(int key) {
		connections_.erase(key);
	}
private:
    int next_key_;
    map<int, cb_func_type>connections_;
};



#endif /* SRC_COMMON_EVENTS_HPP_ */
