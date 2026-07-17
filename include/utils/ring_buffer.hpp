#pragma once

#include <queue>
#include <vector>


template <typename T>
class RingBufferQueue{
private:
    size_t front_index{};
    size_t back_index{};
    size_t queue_size{};
    std::vector<T> main_queue{};
    std::queue<T> backup_queue{};

    void increment_front_index() noexcept;
    void increment_back_index() noexcept;

public:
    void allocate_queue(size_t size, const T& default_value);
    [[nodiscard]] bool empty() const noexcept;
    template<typename... Args>
    void emplace(Args&&... args);
    [[nodiscard]] T& front();
    [[nodiscard]] T const& front() const;
    void pop();
};

#include "ring_buffer_impl.hpp"