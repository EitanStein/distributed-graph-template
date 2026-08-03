#pragma once

#include <cstdint>
#include <atomic>

class NodeStatus{
public:
    NodeStatus() = default;
    NodeStatus(const NodeStatus&) = delete;
    NodeStatus(NodeStatus&& other) noexcept : status_(other.status_.load(std::memory_order_relaxed)) {}

    enum class Status : std::uint8_t {
        None = 0,
        Init = 1,
        SentMsg = 1 << 1,
        ReceivedMsg = 1 << 2
    };

    void updateStatus(Status new_status) noexcept /*Thread safe*/ {
        status_.fetch_or(static_cast<std::uint8_t>(new_status), std::memory_order_relaxed);
    }

    void cycleStatus() noexcept /*NOT thread safe*/ {
        std::uint8_t status = status_.load(std::memory_order_relaxed);
        status_ = static_cast<std::uint8_t>(status << 4);
    }

    [[nodiscard]] bool isLastCycleStatus(Status status) const noexcept /*Thread safe*/ {
        return (status_.load(std::memory_order_relaxed) & (static_cast<std::uint8_t>(status) << 4)) != 0;
    }
private:
    std::atomic<std::uint8_t> status_ = static_cast<std::uint8_t>(Status::None);
};