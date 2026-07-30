#pragma once

#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <stop_token>
#include <vector>
#include <atomic>

#include "ring_buffer.hpp"

namespace ThreadInfo{
    const std::size_t default_thread_count = std::thread::hardware_concurrency();
}

template<std::invocable Task>
class ThreadPool
{
public:
    
    ThreadPool(std::size_t thread_pool_size = ThreadInfo::default_thread_count);
    ThreadPool(const ThreadPool&) = delete;
    ThreadPool(ThreadPool&&) noexcept = delete;
    ThreadPool& operator=(const ThreadPool&) = delete;
    ThreadPool& operator=(ThreadPool&&) noexcept = delete;
    ~ThreadPool();

    [[nodiscard]] bool isTaskQueueEmpty();
    void waitForEmptyQueue();
    void addTask(Task&& task);
private:
    std::size_t thread_pool_size_{};
    std::mutex queue_lock_;
    std::vector<std::jthread> threads_;
    std::condition_variable_any queue_cv_;
    RingBufferQueue<Task> task_queue_;

    std::atomic<std::size_t> num_active_tasks_{};
    std::condition_variable tasks_done_cv_;

    void threadLoop(std::stop_token stoken);
};

#include "thread_pool_impl.hpp"