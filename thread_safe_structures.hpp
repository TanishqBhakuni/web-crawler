#ifndef THREAD_SAFE_STRUCTURES_HPP
#define THREAD_SAFE_STRUCTURES_HPP

#include <queue>
#include <mutex>
#include <condition_variable>
#include <utility>
#include <unordered_set>

template <typename T>
class ThreadSafeQueue{
    public:
    void push(T new_value){
        
        // We use std::lock_guard, a powerful C++ RAII-style mechanism.
        // When 'lk' is created, it automatically calls mtx.lock().
        // This is the start of our "critical section". Only the thread that
        // acquires the lock can proceed past this point.
        std::lock_guard<std::mutex> lk(mtx);

        // Now that we have the lock, we can safely modify the data_queue.
        // We use std::move(new_value) as a performance optimization. Since
        // new_value is a local variable that's about to be destroyed,
        // std::move allows us to "steal" its contents rather than making an
        // expensive copy, which is especially important if 'T' is a large object.
        data_queue.push(std::move(new_value));

        // Step 3: Release the lock.
        // This happens AUTOMATICALLY! When the std::lock_guard 'lk' goes out
        // of scope at the end of this block (at the '}' curly brace), its
        // destructor is called, which automatically calls mtx.unlock().
        // This is safer than manual locking/unlocking because it guarantees
        // the mutex is released, even if an exception were to be thrown.

        // Step 4: Notify a waiting thread.
        // We call notify_one() on our condition variable *after* the lock
        // has been released (or at least just before). This wakes up exactly
        // one thread that might be waiting because the queue was empty.
        // This is a crucial step to "un-block" a consumer thread.
        cv.notify_one();
    }

    T wait_and_pop(){
        std::unique_lock<std::mutex> lk(mtx);
        cv.wait(lk, [this]{ return !this->data_queue.empty(); });
    }

    private:
    mutable std::mutex mtx;
    std::queue<T> data_queue;
    std::condition_variable cv;
    
    T value = std::move(data_queue.front());
    data_queue.pop();
    
    return value;
};

template <typename T>
class ThreadSafeSet {
    public:
    bool add(const T& item){
        std::lock_guard<std::mutex> lk(mtx);
        auto result = data_set.insert(item);
        
        return result.second;
    }

    bool contains(const T& item) const{
        std::lock_guard<std::mutex> lk(mtx);

        return data_set.count(item) > 0;
    }
    
    private:
    mutable std::mutex mtx;
    std::unordered_set<T> data_set;

};

#endif 