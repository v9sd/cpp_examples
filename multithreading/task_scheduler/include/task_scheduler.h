#ifndef _TASH_SCHEDULER_INC_FILE_H__
#define _TASH_SCHEDULER_INC_FILE_H__

#include <map>
#include <functional>
#include <mutex>
#include <thread>
#include <vector>
#include <condition_variable>
#include <chrono>
#include <atomic>

class TaskScheduler{
    public:
        typedef std::function<void()> task_func;
	TaskScheduler(std::size_t threads_amount = 1);

	TaskScheduler(const TaskScheduler & obj) = delete;
	TaskScheduler & operator = (const TaskScheduler & obj) = delete;

	~TaskScheduler();

        void start(TaskScheduler::task_func func_obj = nullptr,
                                  const std::chrono::steady_clock::time_point & t_point =
                                      std::chrono::steady_clock::time_point());
        void start(task_func,
                   const std::chrono::nanoseconds & timeout);

        void setThreadsAmount(std::size_t amount = 0);
        void stop();

    private: //classes
	struct TasksCommon{
		std::multimap<std::chrono::steady_clock::time_point, task_func> tasks;
		std::mutex tasks_lock;
                std::condition_variable_any cv;
	};

        class PrivateThread_p;

	class PrivateThread{
            public:
		PrivateThread(TasksCommon & tasks_common);
		PrivateThread(const PrivateThread & obj) = delete;
		PrivateThread & operator = (const PrivateThread & obj) = delete;

		PrivateThread(PrivateThread && obj);
		PrivateThread & operator = (PrivateThread && obj);

		~PrivateThread();

             private:
                std::unique_ptr<PrivateThread_p> private_p;
	};

    private://methods
	void normalisePrivateThreads();

    private://properties
	TasksCommon _tasks_common;

	std::size_t _threads_amount;
	std::vector<TaskScheduler::PrivateThread> _threads;
};

#endif
