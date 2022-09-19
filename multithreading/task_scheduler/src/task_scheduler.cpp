#include "task_scheduler.h"

class TaskScheduler::PrivateThread_p{
	public: //methods
		PrivateThread_p(TasksCommon & tasks_common) : _tasks_common(tasks_common) {}

		PrivateThread_p(const PrivateThread &) = delete;
		PrivateThread_p & operator = (const PrivateThread &) = delete;
		PrivateThread_p(PrivateThread_p &&) = delete;
		PrivateThread_p & operator = (PrivateThread &&) = delete;
		~PrivateThread_p() = default;

		void operator ()();

	public: //properties
		TasksCommon & _tasks_common;
		std::atomic_flag is_work;
		std::unique_ptr<std::thread> _thread;// = nullptr;
};

class MutexStub{
	public:
		void lock() noexcept {}
		void unlock() noexcept {}
};

void TaskScheduler::PrivateThread_p::operator ()(){
	MutexStub stub_mutex;
	std::pair<decltype(_tasks_common.tasks)::key_type, decltype(_tasks_common.tasks)::mapped_type> cur_value =
		{decltype(_tasks_common.tasks)::key_type(), nullptr};
	while(true){
		std::unique_lock<std::mutex> tasks_lock_locker(_tasks_common.tasks_lock,std::defer_lock);
		tasks_lock_locker.lock(); //tasks lock
		if(cur_value.second != nullptr){
			_tasks_common.tasks.insert({cur_value.first, std::move(cur_value.second)});
			cur_value.second = nullptr;
		}
		if(!is_work.test_and_set()){
			tasks_lock_locker.unlock();//tasks unlock
			break;
		}
		auto first_task = _tasks_common.tasks.begin();
		if (first_task != _tasks_common.tasks.end()){
			cur_value.first = first_task->first;
			cur_value.second = std::move(first_task->second);
			_tasks_common.tasks.erase(first_task);
		}
		tasks_lock_locker.unlock();//tasks unlock
		if (cur_value.second == nullptr){
			_tasks_common.cv.wait(stub_mutex);
		}else{
			auto current_tpoint = std::chrono::steady_clock::now();
			if(cur_value.first <= current_tpoint){
				cur_value.second();
				cur_value.second = nullptr;
			}else{
				std::chrono::steady_clock::duration timeout = cur_value.first - current_tpoint;
				if(_tasks_common.cv.wait_for(stub_mutex, timeout) == std::cv_status::timeout){
					cur_value.second();
					cur_value.second = nullptr;
				}
			}
		}
	}
}

TaskScheduler::PrivateThread::PrivateThread(TaskScheduler::TasksCommon & tasks_common) : private_p(new TaskScheduler::PrivateThread_p(tasks_common)) {
	private_p->is_work.test_and_set();
	private_p->_thread.reset(new std::thread([](auto f_obj_p){ (*f_obj_p)();}, private_p.get()));
}

TaskScheduler::PrivateThread::PrivateThread(PrivateThread && obj) {
	std::swap(private_p, obj.private_p);
}

TaskScheduler::PrivateThread & TaskScheduler::PrivateThread::operator = (PrivateThread && obj){
	std::swap(private_p, obj.private_p);
	return *this;
}

TaskScheduler::PrivateThread::~PrivateThread(){
	if(!private_p)
		return;

	private_p->is_work.clear();
	private_p->_tasks_common.cv.notify_all();
	private_p->_thread->join();
}

void TaskScheduler::setThreadsAmount(std::size_t amount){
	_threads_amount = amount;
	normalisePrivateThreads();
}

TaskScheduler::TaskScheduler(std::size_t threads_amount) : _threads_amount(threads_amount) {}

void TaskScheduler::start(TaskScheduler::task_func func_obj, const std::chrono::steady_clock::time_point  & t_point){
	if(func_obj != nullptr){
		std::unique_lock<std::mutex> unique_lock(_tasks_common.tasks_lock,std::defer_lock);
		unique_lock.lock();
		_tasks_common.tasks.insert({t_point,func_obj});
		_tasks_common.cv.notify_one();
		unique_lock.unlock();
	}
	normalisePrivateThreads();
}

void TaskScheduler::start(TaskScheduler::task_func func_obj, const std::chrono::nanoseconds & timeout){

	std::chrono::steady_clock::time_point t_point;//this is not initializing because that default t_point duration value is zero
	if(timeout != std::chrono::nanoseconds::zero())
		t_point = std::chrono::steady_clock::now() + timeout;
	start(func_obj, t_point);
}

void TaskScheduler::stop(){
	_threads.clear();
}

void TaskScheduler::normalisePrivateThreads(){
	std::size_t cur_size = _threads.size();
	if(_threads_amount < cur_size){
		_threads.erase(_threads.end() - (cur_size - _threads_amount), _threads.end());
		return;
	}

	std::size_t addeitional_thr_amount =  _threads_amount - cur_size;
	_threads.reserve(_threads_amount);
	while(addeitional_thr_amount--)
		_threads.emplace_back(_tasks_common);
}

TaskScheduler::~TaskScheduler(){
	stop();
}
