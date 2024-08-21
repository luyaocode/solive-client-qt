#include "stdafx.h"
#include "ThreadPool.h"

using namespace SoLive::Util;

ThreadPool& ThreadPool::instance()
{
    static ThreadPool threadPool;
    return threadPool;
}

ThreadPool::ThreadPool(size_t numThreads) : _stop(false)
{
    for (size_t i = 0; i < numThreads; ++i)
    {
        _workers.emplace_back(&ThreadPool::worker, this);
    }
}

ThreadPool::~ThreadPool()
{
    std::lock_guard<std::mutex> lock(_queueMutex);
    _stop = true;
    _condition.notify_all();

    for (std::thread& worker : _workers)
    {
        worker.join();
    }
}

void ThreadPool::enqueue(std::function<void()> task)
{
    {
        std::lock_guard<std::mutex> lock(_queueMutex);
        _tasks.push(task);
    }
    _condition.notify_one();
}

void ThreadPool::worker()
{
    while (true)
    {
        std::function<void()> task;
        {
            std::unique_lock<std::mutex> lock(_queueMutex);
            _condition.wait(lock, [this]() { return _stop || !_tasks.empty(); });
            if (_stop && _tasks.empty())
            {
                return;
            }
            task = std::move(_tasks.front());
            _tasks.pop();
        }
        task();
    }
}
