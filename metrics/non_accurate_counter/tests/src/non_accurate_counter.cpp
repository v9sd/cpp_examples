#include <non_accurate_counter.h>

#include <gtest/gtest.h>

TEST(NonAccurateCounter, Constructor) {

	std::chrono::steady_clock::time_point start_time = std::chrono::steady_clock::now();
	NonAccurateCounter counter(std::chrono::seconds(1), start_time);
	ASSERT_EQ(u_int64_t(counter), 0);
}

TEST(NonAccurateCounter, Add) {

	std::chrono::steady_clock::time_point current_time = std::chrono::steady_clock::now();
	NonAccurateCounter counter(std::chrono::seconds(1), current_time);
	ASSERT_EQ(u_int64_t(counter), 0);

	current_time += std::chrono::seconds(1);
	counter.add(10, current_time);
	ASSERT_EQ(u_int64_t(counter), 10);//octave: uint64(0*1) + 10 = 10

	current_time += std::chrono::milliseconds(100);
	counter.add(10, current_time);
	ASSERT_EQ(u_int64_t(counter), 19);//octave: uint64(10*0.9) + 10 = 19

	current_time += std::chrono::milliseconds(200);
	counter.add(80, current_time);
	ASSERT_EQ(u_int64_t(counter), 95);//octave: uint64(19*0.8)+80 = 95

	current_time += std::chrono::milliseconds(800);
	counter.add(0, current_time);
	ASSERT_EQ(u_int64_t(counter), 19);//octave: uint64(95*0.2)+0 = 19
}

