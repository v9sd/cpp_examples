#ifndef _NON_ACCURATE_COUNTER_INC_FILE_H__
#define _NON_ACCURATE_COUNTER_INC_FILE_H__

#include <chrono>
#include <list>

class AccurateCounter{
    public:
        AccurateCounter(std::chrono::steady_clock::duration interval = std::chrono::seconds(1),
                        std::chrono::steady_clock::time_point current_time = std::chrono::steady_clock::now());
        AccurateCounter & add(uint64_t amount = 0,
                 std::chrono::steady_clock::time_point current_time = std::chrono::steady_clock::now());
        operator uint64_t () const noexcept;

    private://types
        typedef std::pair<std::chrono::steady_clock::time_point, uint64_t> AddPoint;
    private:
        std::chrono::steady_clock::duration _interval;
        std::list<AddPoint> _points;
        uint64_t _amount = 0;
};

#endif
