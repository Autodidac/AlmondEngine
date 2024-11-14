#pragma once

#include "WaitFreeQueue.h"
#include "Exports_DLL.h"

#include <atomic>
#include <functional>
#include <memory>     // Include for unique_ptr
#include <thread>
#include <vector>

namespace almond
{
    class ThreadPool {
    public:
        explicit ThreadPool(size_t threadCount);
        ~ThreadPool();

        void enqueue(std::function<void()> job);

    private:
        int workerThread(); // Worker function for threads

        std::vector<std::thread> workers; // Worker threads
        std::unique_ptr<WaitFreeQueue<std::function<void()>>> jobQueue; // Job queue
        std::unique_ptr<std::atomic<bool>> isRunning; // Running status
    };

    // ThreadPool constructor, destructor, and methods are defined inline in this header file.
    inline ThreadPool::ThreadPool(size_t threadCount)
        : jobQueue(std::make_unique<WaitFreeQueue<std::function<void()>>>(threadCount)),
        isRunning(std::make_unique<std::atomic<bool>>(true)) {
        for (size_t i = 0; i < threadCount; ++i) {
            workers.emplace_back(&ThreadPool::workerThread, this);
        }
    }

    inline ThreadPool::~ThreadPool() {
        *isRunning = false; // Signal all threads to stop
        for (auto& worker : workers) {
            worker.join(); // Wait for all worker threads to finish
        }
    }

    inline void ThreadPool::enqueue(std::function<void()> job) {
        jobQueue->enqueue(std::move(job)); // Enqueue the job
    }

    inline int ThreadPool::workerThread() {
        while (*isRunning) {
            std::function<void()> job;
            if (jobQueue->dequeue(job)) {  // Non-blocking dequeue
                if (job) {
                    job();  // Execute the job
                }
            }
            else {
                std::this_thread::yield(); // Yield CPU if no job is available
            }
        }

        // Drain the job queue
        std::function<void()> job;
        while (jobQueue->dequeue(job)) {
            if (job) {
                job();  // Execute remaining jobs
            }
        }
        return 0;
    }

} // namespace almond
/*
// Export functions for DLL use
extern "C" {

    almond::ThreadPool* createThreadPool(size_t threadCount) {
        return new almond::ThreadPool(threadCount);
    }

    void destroyThreadPool(almond::ThreadPool* pool) {
        delete pool;
    }

    void enqueueJob(almond::ThreadPool* pool, std::function<void()> job) {
        pool->enqueue(std::move(job));
    }

}
*/