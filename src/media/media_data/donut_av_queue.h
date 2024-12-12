//#ifndef DONUT_AV_QUEUE
//#define DONUT_AV_QUEUE
//#include <queue>
//#include <mutex>
//
//namespace Donut
//{
//template <typename T>
//class DonutAVQueue
//{
//public:
//	DonutAVQueue()
//	{
//
//	}
//
//	~DonutAVQueue()
//	{
//		std::lock_guard<std::mutex> lock(mtx_);
//		while (getSize() > 0)
//		{
//			queue_.pop();
//		}
//	}
//
//	int push(T* data)
//	{
//		std::lock_guard<std::mutex> lock(mtx_);
//		queue_.push(data);
//		return 0;
//	}
//
//	int pop(T** data)
//	{
//		int size = getSize();
//		if (size > 0)
//		{
//			std::lock_guard<std::mutex> lock(mtx_);
//			*data = queue_.front();
//			queue_.pop();
//			return 0;
//		}
//		return -1;
//	}
//
//	int getSize()
//	{
//		return queue_.size();
//	}
//private:
//	std::queue<T*> queue_;
//	std::mutex mtx_;
//};
//}
//#endif