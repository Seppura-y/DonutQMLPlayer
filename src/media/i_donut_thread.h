#ifndef I_DONUT_THREAD_H
#define I_DONUT_THRAED_H

#include <thread>
#include <mutex>

class IDonutThread
{
public:
	IDonutThread() = default;
	virtual ~IDonutThread() {}

	virtual void start();
	virtual void stop();
	virtual void pause();

	bool isExit() { return is_exit_; }
protected:
	virtual void threadLoop() = 0;

protected:
	std::mutex mtx_;
	std::thread worker_;

	bool is_exit_ = true;
	bool is_pause_ = false;

	int thread_index_ = 0;
};


#endif