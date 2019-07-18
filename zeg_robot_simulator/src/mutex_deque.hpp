#ifndef SRC_MUTEX_DEQUE_HPP_
#define SRC_MUTEX_DEQUE_HPP_
#include <vector>
#include <deque>
#include <mutex>
#include <memory>
using namespace std;
enum {
	DEQUE_IS_EMPTY,
	DEQUE_IS_FULL,
	DEQUE_IS_UPDATED,
	DEQUE_IS_UNUPDATED
};
template <class T>
class mutex_deque {
public:
	mutex_deque() : mutex_ptr_(new mutex) {
		max_size_ = 1024;
		status_ = DEQUE_IS_EMPTY;
	}
	mutex_deque(const mutex_deque &other) {
		max_size_ = other.max_size_;
		status_ = other.status_;
	}
	virtual ~mutex_deque() = default;
public:
	inline void set_max_size(int size) {
		max_size_ = size;
	}
	bool push_back(const T &e) {
		lock_guard<mutex>lk(*mutex_ptr_);
		if (deque_.size() >= max_size_) {
			status_ = DEQUE_IS_FULL;
			return false;
		}
		deque_.emplace_back(e);
		return true;
	}
	bool pop_front(T &e) {
		lock_guard<mutex>lk(*mutex_ptr_);
		if (deque_.empty()) {
			status_ = DEQUE_IS_EMPTY;
			return false;
		}
		e = deque_.front();
		deque_.pop_front();
		return true;
	}
	inline bool empty() {
		lock_guard<mutex>lk(*mutex_ptr_);
		return deque_.empty();
	}
	bool update_deque(const vector<T>&vec) {
		int count = 0;
		lock_guard<mutex>lk(*mutex_ptr_);
		deque_.clear();
		for (auto &e : vec) {
			deque_.emplace_back(e);
			if (++count >= max_size_) {
				break;
			}
		}
		status_ = DEQUE_IS_UPDATED;
		return deque_.size() > 0;
	}
	inline unsigned char get_status() const {
		lock_guard<mutex>lk(*mutex_ptr_);
		return status_;
	}
	inline void set_status(unsigned char status) {
		lock_guard<mutex>lk(*mutex_ptr_);
		status_ = status;
	}
private:
	int max_size_;
	unsigned char status_;
	deque<T>deque_;
	unique_ptr<mutex>mutex_ptr_;
};



#endif /* SRC_MUTEX_DEQUE_HPP_ */
