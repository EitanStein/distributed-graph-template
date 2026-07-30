#pragma once

#include <queue>
#include <vector>


template <typename T>
class RingBufferQueue{
public:
    void allocate_queue(size_t size, const T& default_value);
    [[nodiscard]] bool empty() const noexcept;
    template<typename... Args>
    void emplace(Args&&... args);
    [[nodiscard]] T& front();
    [[nodiscard]] T const& front() const;
    void pop();

private:
    size_t front_index_{};
    size_t back_index_{};
    size_t queue_size_{};
    std::vector<T> main_queue_{};
    std::queue<T> backup_queue_{};

    void increment_front_index() noexcept;
    void increment_back_index() noexcept;

};

#include "ring_buffer_impl.hpp"