#include <accurate_counter.h>

#include <gtest/gtest.h>

TEST(AccurateCounter, Constructor) {

	std::chrono::steady_clock::time_point start_time = std::chrono::steady_clock::now();
	AccurateCounter counter(std::chrono::seconds(1), start_time);
	ASSERT_EQ(u_int64_t(counter), 0);
}

TEST(AccurateCounter, Add) {

	std::chrono::steady_clock::time_point current_time = std::chrono::steady_clock::now();
	AccurateCounter counter(std::chrono::seconds(1), current_time);
	ASSERT_EQ(u_int64_t(counter), 0);//start_point

	current_time += std::chrono::seconds(1);//1000 milliseconds
	counter.add(10, current_time);
	ASSERT_EQ(u_int64_t(counter), 10);//octave: 10 = 10

	current_time += std::chrono::milliseconds(100);
	counter.add(10, current_time);
	ASSERT_EQ(u_int64_t(counter), 19);//octave: 10+10*0.9 = 19

	current_time += std::chrono::milliseconds(200);
	counter.add(80, current_time);
	/* |-- 200 --|-- 100 --|-- 1000 --|--
	 * |   80    |   10    |   10     |
	 */
	ASSERT_EQ(u_int64_t(counter), 97);//octave: 80+10+uint64(10*0.7) = 97

	current_time += std::chrono::milliseconds(800);
	counter.add(0, current_time);
	/* |-- 800 --|-- 200 --|--
	 * |   0     |   80    |
	 */
	ASSERT_EQ(u_int64_t(counter), 80); //0+80 = 80
}

