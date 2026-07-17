#pragma once
#include "ring_buffer.hpp"

template<typename T>
void RingBufferQueue<T>::increment_front_index() noexcept{
    ++front_index;
    front_index = (front_index != main_queue.size()) ? front_index : 0;
}

template<typename T>
void RingBufferQueue<T>::increment_back_index() noexcept{
    ++back_index;
    back_index = (back_index != main_queue.size()) ? back_index : 0;
}

template<typename T>
void RingBufferQueue<T>::allocate_queue(size_t size, const T& default_value) {
    main_queue = std::vector<T>(size, default_value);
}

template<typename T>
[[nodiscard]] bool RingBufferQueue<T>::empty() const noexcept{
    return queue_size == 0;
}

template<typename T>
template<typename ...Args>
void RingBufferQueue<T>::emplace(Args&&... args){
    if(queue_size < main_queue.size()){
        // TODO placement new to avoid temporary constructor call
        main_queue[back_index] = T(std::forward<Args>(args)...);
        increment_back_index();
    }
    else
        backup_queue.emplace(std::forward<Args>(args)...);

    ++queue_size;
}

template<typename T>
[[nodiscard]] T& RingBufferQueue<T>::front(){
    if(queue_size == 0) [[unlikely]]
        throw std::runtime_error("empty task queue");
    return main_queue[front_index];
}

template<typename T>
[[nodiscard]] T const& RingBufferQueue<T>::front() const{
    if(queue_size == 0) [[unlikely]]
        throw std::runtime_error("empty task queue");
    return main_queue.at(front_index);
}

template<typename T>
void RingBufferQueue<T>::pop(){
    if(queue_size == 0) [[unlikely]] // TODO remove?
        return;
    increment_front_index();
    
    if(!backup_queue.empty()){
        main_queue[back_index] = std::move(backup_queue.front());
        backup_queue.pop();
        increment_back_index();
    }

    --queue_size;
}