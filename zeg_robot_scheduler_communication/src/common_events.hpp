#ifndef SRC_COMMON_EVENTS_HPP_
#define SRC_COMMON_EVENTS_HPP_
#include <map>
using namespace std;
template <class cb_func_type>
class common_events {
public:
	common_events() {
		next_key_ = 0;
	}
	common_events(const common_events &) = delete;
	common_events & operator = (const common_events &) = delete;
	virtual ~common_events() = default;
public:
	inline int operator += (cb_func_type &cb_fun) {
        return connect(cb_fun);
    }
	inline int operator += (cb_func_type &&cb_fun) {
        return connect(forward<cb_func_type>(cb_fun));
    }
    template <typename... Args>
    bool operator()(Args&&... args) {
    	return notify(forward<Args>(args)...);
    }
    inline common_events& operator -= (int key) {
    	disconnect(key);
    	return *this;
    }
    void clear() {
    	connections_.clear();
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
    template <typename... Args>
    bool notify(Args&&... args) {
    	bool succ = true;
    	for (auto& it : connections_) {
    		if (false == it.second(forward<Args>(args)...)) {
    			succ = false;
    		}
        }
    	return succ;
    }
private:
    int next_key_;
    map<int, cb_func_type>connections_;
};

#endif /* SRC_COMMON_EVENTS_HPP_ */
