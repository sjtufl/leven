#ifndef LEVEN_THREADPOOL_H
#define LEVEN_THREADPOOL_H

#include <mutex>
#include <condition_variable>
#include <atomic>
#include <thread>
#include <vector>
#include <deque>

#include <leven/noncopyable.h>
#include <leven/Callbacks.h>

namespace leven
{

class ThreadPool {
public:
    explicit ThreadPool(size_t numThread,
                        size_t maxQueueSize = 65536,
                        const ThreadInitCallback& cb = nullptr);
    ~ThreadPool();

    void runTask(const Task& task);
    void runTask(Task&& task);
    void stop();
    size_t numThreads() const {
        return threads_.size();
    }

private:
    void runInThread(size_t index);
    Task takeTask();

private:

    typedef std::unique_ptr<std::thread> ThreadPtr;
    typedef std::vector<ThreadPtr> ThreadList;

    ThreadList threads_;
    std::mutex mutex_;
    std::condition_variable nonEmpty_;
    std::condition_variable nonFull_;

    std::deque<Task> taskQueue_;
    const size_t maxQueueSize_;
    std::atomic_bool running_;
    ThreadInitCallback threadInitCallback_;
};

} // namespace leven


#endif //LEVEN_THREADPOOL_H
