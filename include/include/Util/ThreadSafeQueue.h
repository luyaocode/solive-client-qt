#ifndef THREADSAFEQUEUE_H
#define THREADSAFEQUEUE_H
#include <queue>
#include <mutex>
#include <condition_variable>

namespace SoLive::Util
{
    template <typename T>
    class ThreadSafeQueue
    {
    public:
        void push(const T& item);
        bool try_pop(T& item);
        void wait_and_pop(T& item);
        inline bool isEmpty() const { return _queue.empty(); }
    private:
        std::queue<T> _queue;
        std::mutex _mutex;
        std::condition_variable _condition;
    };
}
#include "ThreadSafeQueueImpl.h"

#endif // THREADSAFEQUEUE_H