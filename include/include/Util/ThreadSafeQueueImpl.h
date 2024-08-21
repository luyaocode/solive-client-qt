#ifndef THREADSAFEQUEUEIMPL_H
#define THREADSAFEQUEUEIMPL_H

#include "ThreadSafeQueue.h"

namespace SoLive::Util
{
    template <typename T>
    void ThreadSafeQueue<T>::push(const T& item)
    {
        std::lock_guard<std::mutex> lock(_mutex);
        _queue.push(item);
        _condition.notify_one(); // Notify one waiting thread
    }

    template <typename T>
    bool ThreadSafeQueue<T>::try_pop(T& item)
    {
        std::lock_guard<std::mutex> lock(_mutex);
        if (_queue.empty())
            return false;
        item = _queue.front();
        _queue.pop();
        return true;
    }

    template <typename T>
    void ThreadSafeQueue<T>::wait_and_pop(T& item)
    {
        std::unique_lock<std::mutex> lock(_mutex);
        _condition.wait(lock, [this] { return !_queue.empty(); }); // Wait until queue is not empty
        item = _queue.front();
        _queue.pop();
    }
}

#endif // THREADSAFEQUEUEIMPL_H