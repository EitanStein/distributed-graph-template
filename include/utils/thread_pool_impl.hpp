#pragma once

#include "thread_pool.hpp"

template<std::invocable Task>
ThreadPool<Task>::ThreadPool(std::size_t thread_pool_size) : thread_pool_size(thread_pool_size), num_active_tasks(0){
    threads.reserve(thread_pool_size);
    for(std::size_t i=0; i < thread_pool_size ; ++i);{
        threads.emplace_back([this](std::stop_token stoken){ this->threadLoop(stoken); });
    } 
}

template<std::invocable Task>
ThreadPool<Task>::~ThreadPool() {
    for(auto& th : threads){
        th.request_stop();
    }

    queue_cv.notify_all();
}

template<std::invocable Task>
bool ThreadPool<Task>::isTaskQueueEmpty(){
    bool result;
    {
        std::lock_guard<std::mutex> lock(queue_lock);
        result = task_queue.empty();
    }

    if(!result)
        queue_cv.notify_one();

    return result;
}

template<std::invocable Task>
void ThreadPool<Task>::addTask(Task&& task){
    {
        std::lock_guard<std::mutex> lock(queue_lock);
        task_queue.emplace(std::forward<Task>(task));
    }
    queue_cv.notify_one();
}

template<std::invocable Task>
void ThreadPool<Task>::waitForEmptyQueue(){
    std::unique_lock<std::mutex> lock(queue_lock);
    tasks_done_cv.wait(lock, [this](){
        return this->task_queue.empty() && num_active_tasks == 0;
    });
}


template<std::invocable Task>
void ThreadPool<Task>::threadLoop(std::stop_token stoken){
    std::optional<Task> cur_task = std::nullopt;
    while(!stoken.stop_requested()){
        {
            std::unique_lock<std::mutex> lock(queue_lock);

            // TODO refactor?
            // maybe after task is done reduce atomic val and and check only if the atomic value is 0
            if(cur_task != std::nullopt){
                --num_active_tasks;

                if(task_queue.empty() && num_active_tasks == 0){
                    tasks_done_cv.notify_all();
                }
                cur_task = std::nullopt;
            }

            queue_cv.wait(lock, stoken, [this](){
                return !this->task_queue.empty();
            });

            if(stoken.stop_requested())
                return;
            
            ++num_active_tasks;
            cur_task = task_queue.front();
            task_queue.pop();
        }

        cur_task.value()();

    }
}
