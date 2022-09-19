#include "non_accurate_counter.h"

#include <stdexcept>

NonAccurateCounter::NonAccurateCounter(std::chrono::steady_clock::duration interval,
				       std::chrono::steady_clock::time_point start_time) : _interval(interval), _prev_time(start_time) { }

NonAccurateCounter& NonAccurateCounter::add(uint64_t amount,
			     std::chrono::steady_clock::time_point current_time) {
	if (_prev_time > current_time)
		throw std::invalid_argument("current time less than prev");

	std::chrono::steady_clock::duration gape = current_time - _prev_time;
	if(gape >= _interval){
		_amount = amount * ((double)_interval.count() / (double)gape.count());
	}else{
		_amount *= ((double)(_interval - gape).count() / (double)_interval.count());
		_amount += amount;
	}
	_prev_time = current_time;
	return *this;
}

NonAccurateCounter::operator uint64_t () const noexcept {
	return _amount;
}
