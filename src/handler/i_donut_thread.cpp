#include "i_donut_thread.h"

#include <iostream>
#include <sstream>

namespace Donut
{
    void IDonutThread::start()
    {
        static int i = 0;
        std::lock_guard<std::mutex> lock(mtx_);

        this->thread_index_ = i++;
        is_exit_ = false;

        worker_ = std::thread(&IDonutThread::threadLoop, this);

        std::stringstream stream;
        stream << "thread " << std::this_thread::get_id() << " : start";
    }

    void IDonutThread::stop()
    {
        std::stringstream stream;
        stream << "thread " << std::this_thread::get_id() << " : request stop";

        is_exit_ = true;
        if (worker_.joinable())
        {
            worker_.join();
        }
        stream.clear();
        stream << "thread " << std::this_thread::get_id() << " : stop";

    }

    void IDonutThread::pause()
    {
        std::unique_lock<std::mutex> lock(mtx_);
        this->is_pause_ = !this->is_pause_;
    }

}