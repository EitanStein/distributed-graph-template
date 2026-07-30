#pragma once

#include "thread_pool.hpp"

template<std::invocable Task>
ThreadPool<Task>::ThreadPool(std::size_t thread_pool_size) : thread_pool_size_(thread_pool_size), num_active_tasks_(0){
    threads_.reserve(thread_pool_size_);
    for(std::size_t i=0; i < thread_pool_size_ ; ++i){
        threads_.emplace_back([this](std::stop_token stoken){ this->threadLoop(stoken); });
    } 
}

template<std::invocable Task>
ThreadPool<Task>::~ThreadPool() {
    for(auto& th : threads_){
        th.request_stop();
    }

    queue_cv_.notify_all();
}

template<std::invocable Task>
bool ThreadPool<Task>::isTaskQueueEmpty(){
    bool result;
    {
        std::lock_guard<std::mutex> lock(queue_lock_);
        result = task_queue_.empty();
    }

    if(!result)
        queue_cv_.notify_one();

    return result;
}

template<std::invocable Task>
void ThreadPool<Task>::addTask(Task&& task){
    {
        std::lock_guard<std::mutex> lock(queue_lock_);
        task_queue_.emplace(std::forward<Task>(task));
    }
    queue_cv_.notify_one();
}

template<std::invocable Task>
void ThreadPool<Task>::waitForEmptyQueue(){
    std::unique_lock<std::mutex> lock(queue_lock_);
    tasks_done_cv_.wait(lock, [this](){
        return this->task_queue_.empty() && num_active_tasks_ == 0;
    });
}


template<std::invocable Task>
void ThreadPool<Task>::threadLoop(std::stop_token stoken){
    std::optional<Task> cur_task = std::nullopt;
    while(!stoken.stop_requested()){
        {
            std::unique_lock<std::mutex> lock(queue_lock_);

            // TODO refactor?
            // maybe after task is done reduce atomic val and and check only if the atomic value is 0
            if(cur_task != std::nullopt){
                --num_active_tasks_;

                if(task_queue_.empty() && num_active_tasks_ == 0){
                    tasks_done_cv_.notify_all();
                }
                cur_task = std::nullopt;
            }

            queue_cv_.wait(lock, stoken, [this](){
                return !this->task_queue_.empty();
            });

            if(stoken.stop_requested())
                return;
            
            ++num_active_tasks_;
            cur_task = task_queue_.front();
            task_queue_.pop();
        }

        cur_task.value()();

    }
}
