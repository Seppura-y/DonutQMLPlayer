#ifndef DONUT_TIMER_H
#define DONUT_TIMER_H

namespace Donut
{
	class DonutTimer
	{
	public:
		static long long getTime();
	};

	class TimeStep
	{
	public:
		TimeStep(long long time = 0) : time_(0) {}

		operator long long() const { return time_; }

		long long getSeconds() const { return time_; }
		long long getMilliseconds() const { return time_ * 1000; }
	private:
		long long time_;
	};
}

#endif