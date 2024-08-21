#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <functional>
#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>

namespace SoLive::Util
{
    class ThreadPool
    {
    public:
        static ThreadPool& instance();
        void enqueue(std::function<void()> task);
    private:
        ThreadPool(size_t numThreads = 10);
        ~ThreadPool();
        void worker();
        std::vector<std::thread> _workers;
        std::queue<std::function<void()>> _tasks;
        std::mutex _queueMutex;
        std::condition_variable _condition;
        bool _stop;
    };
}

#endif // THREADPOOL_H

