#include "accurate_counter.h"

#include <stdexcept>

AccurateCounter::AccurateCounter(std::chrono::steady_clock::duration interval,
				 std::chrono::steady_clock::time_point current_time) : _interval(interval) {
	_points.push_back({current_time, 0});
}

AccurateCounter& AccurateCounter::add(uint64_t amount,
			     std::chrono::steady_clock::time_point current_time) {
	auto first_point = _points.begin();
	if(first_point->first >  current_time){
		 throw std::invalid_argument("current time less than prev");
	}
	first_point = _points.insert(first_point, {current_time, amount});

	uint64_t new_amount = 0;
	auto second_point = ++_points.begin();
	for(std::chrono::steady_clock::duration current_period = std::chrono::steady_clock::duration::zero() ;
	    second_point != _points.end(); ++first_point, ++second_point){
		std::chrono::steady_clock::duration gap = first_point->first - second_point->first;
		std::chrono::steady_clock::duration cur_plus_gap = current_period + gap;
		if(cur_plus_gap < _interval || gap == std::chrono::steady_clock::duration::zero()){
			new_amount += first_point->second;
			current_period += gap;
		}else if(cur_plus_gap > _interval){
			std::chrono::steady_clock::duration tail = gap - (cur_plus_gap - _interval);
			double mult = (double)tail.count() / (double)gap.count();
			new_amount += first_point->second * mult;
			++second_point;
			break;
		}else if(cur_plus_gap == _interval){
			new_amount += first_point->second;
			++second_point;
			break;
		}
	}
	_points.erase(second_point, _points.end());
	_amount = new_amount;
	return *this;
}

AccurateCounter::operator uint64_t () const noexcept {
	return _amount;
}
