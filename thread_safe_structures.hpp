#ifndef THREAD_SAFE_STRUCTURES_HPP
#define THREAD_SAFE_STRUCTURES_HPP

#include <queue>
#include <mutex>
#include <condition_variable>

template <typename T>
class ThreadSafeQueue{
    public:

    private:
    mutable std::mutex mtx;
    std::queue<T> data_queue;
    std::condition_variable cv;
};

#endif 