#include <cassert>

#include <leven/ThreadPool.h>
#include <leven/Logger.h>

using namespace leven;

ThreadPool::ThreadPool(size_t numThread, size_t maxQueueSize, const leven::ThreadInitCallback &cb)
            : maxQueueSize_(maxQueueSize),
              running_(true),
              threadInitCallback_(cb)
{
    assert(maxQueueSize > 0);
    for (size_t i = 1; i <= numThread; ++i) {
        threads_.emplace_back(new std::thread([this, i](){runInThread(i);}));
    }
    TRACE("ThreadPool::ThreadPool() numThreads: %lu, maxQueueSize: %lu",
            numThread, maxQueueSize);
}

ThreadPool::~ThreadPool()
{
    if (running_)
        stop();
    TRACE("ThreadPool::~ThreadPool()");
}

void ThreadPool::stop()
{
    assert(running_);
    running_ = false;
    // make use of std::lock_guard; automatic mutex_ management
    {
        std::lock_guard<std::mutex> guard(mutex_);
        nonEmpty_.notify_all();
    }
    for (auto& thread: threads_) {
        thread->join();
    }
}

void ThreadPool::runTask(const leven::Task &task)
{
    assert(running_);

    if (threads_.empty()) {
        task();
    } else {
        std::unique_lock<std::mutex> lock(mutex_);
        // while taskQueue_ cannot accept more tasks
        while (taskQueue_.size() >= maxQueueSize_)
            nonFull_.wait(lock);
        taskQueue_.push_back(task);
        // Wake up one randomly
        nonEmpty_.notify_one();
    }
}

void ThreadPool::runTask(Task&& task)
{
    assert(running_);

    if (threads_.empty()) {
        task();
    } else {
        std::unique_lock<std::mutex> lock(mutex_);

        // DEBUG("taskQueue.size(): %lu", taskQueue_.size());
        // while taskQueue_ cannot accept more tasks
        while (taskQueue_.size() >= maxQueueSize_)
            nonFull_.wait(lock);
        taskQueue_.push_back(std::move(task));
        // Wake up one randomly
        nonEmpty_.notify_one();
    }
}

void ThreadPool::runInThread(size_t index)
{
    if (threadInitCallback_)
        threadInitCallback_(index);
    while (running_) {
        if (Task task = takeTask())
            task();
    }
}

Task ThreadPool::takeTask()
{
    std::unique_lock<std::mutex> lock(mutex_);
    while (taskQueue_.empty() && running_)
        nonEmpty_.wait(lock);

    Task task;
    if (!taskQueue_.empty()) {
        task = taskQueue_.front();
        taskQueue_.pop_front();
        nonFull_.notify_one();
    }
    return task;
}
