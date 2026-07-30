#pragma once
#include "ring_buffer.hpp"

template<typename T>
void RingBufferQueue<T>::increment_front_index() noexcept{
    ++front_index_;
    front_index_ = (front_index_ != main_queue_.size()) ? front_index_ : 0;
}

template<typename T>
void RingBufferQueue<T>::increment_back_index() noexcept{
    ++back_index_;
    back_index_ = (back_index_ != main_queue_.size()) ? back_index_ : 0;
}

template<typename T>
void RingBufferQueue<T>::allocate_queue(size_t size, const T& default_value) {
    main_queue_ = std::vector<T>(size, default_value);
}

template<typename T>
[[nodiscard]] bool RingBufferQueue<T>::empty() const noexcept{
    return queue_size_ == 0;
}

template<typename T>
template<typename ...Args>
void RingBufferQueue<T>::emplace(Args&&... args){
    if(queue_size_ < main_queue_.size()){
        // TODO placement new to avoid temporary constructor call
        main_queue_[back_index_] = T(std::forward<Args>(args)...);
        increment_back_index();
    }
    else
        backup_queue_.emplace(std::forward<Args>(args)...);

    ++queue_size_;
}

template<typename T>
[[nodiscard]] T& RingBufferQueue<T>::front(){
    if(queue_size_ == 0) [[unlikely]]
        throw std::runtime_error("empty task queue");
    return main_queue_[front_index_];
}

template<typename T>
[[nodiscard]] T const& RingBufferQueue<T>::front() const{
    if(queue_size_ == 0) [[unlikely]]
        throw std::runtime_error("empty task queue");
    return main_queue_.at(front_index_);
}

template<typename T>
void RingBufferQueue<T>::pop(){
    if(queue_size_ == 0) [[unlikely]] // TODO remove?
        return;
    increment_front_index();
    
    if(!backup_queue_.empty()){
        main_queue_[back_index_] = std::move(backup_queue_.front());
        backup_queue_.pop();
        increment_back_index();
    }

    --queue_size_;
}