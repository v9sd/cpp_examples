#ifndef _NON_ACCURATE_COUNTER_INC_FILE_H__
#define _NON_ACCURATE_COUNTER_INC_FILE_H__

#include <chrono>

class NonAccurateCounter{
    public:
        NonAccurateCounter(std::chrono::steady_clock::duration interval = std::chrono::seconds(1),
                           std::chrono::steady_clock::time_point start_time = std::chrono::steady_clock::now());
        NonAccurateCounter & add(uint64_t amount = 0,
                 std::chrono::steady_clock::time_point current_time = std::chrono::steady_clock::now());
        operator uint64_t () const noexcept;

    private:
        std::chrono::steady_clock::duration _interval;
        std::chrono::steady_clock::time_point _prev_time;
        uint64_t _amount = 0;
};

#endif
