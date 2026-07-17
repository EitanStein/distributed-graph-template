#pragma once

#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <stop_token>
#include <vector>
#include <atomic>

#include "ring_buffer.hpp"


const std::size_t default_thread_count = std::thread::hardware_concurrency();


template<std::invocable Task>
class ThreadPool
{
private:
    std::size_t thread_pool_size{};
    std::mutex queue_lock;
    std::vector<std::jthread> threads;
    std::condition_variable_any queue_cv;
    RingBufferQueue<Task> task_queue;

    std::atomic<std::size_t> num_active_tasks{};
    std::condition_variable tasks_done_cv;

    void ThreadLoop(std::stop_token stoken) {};
public:
    ThreadPool(std::size_t thread_pool_size = default_thread_count);
    ThreadPool(const ThreadPool&) = delete;
    ThreadPool(ThreadPool&&) noexcept = delete;
    ThreadPool& operator=(const ThreadPool&) = delete;
    ThreadPool& operator=(ThreadPool&&) noexcept = delete;
    ~ThreadPool();

    [[nodiscard]] bool IsTaskQueueEmpty(){return true;};
    void WaitForEmptyQueue();
    void AddTask(Task task);
};

#include "thread_pool_impl.hpp"