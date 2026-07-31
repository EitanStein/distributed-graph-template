#pragma once

#include <cstdint>

class NodeStatus{
public:
    enum class Status : std::uint8_t {
        None = 0,
        Init = 1,
        SentMsg = 1 << 1,
        ReceivedMsg = 1 << 2
    };

    void updateStatus(Status new_status) noexcept {
        status_ |= static_cast<std::uint8_t>(new_status);
    }

    void cycleStatus() noexcept {
        status_ = status_ << 4;
    }

    [[nodiscard]] bool isLastCycleStatus(Status status) const noexcept {
        return (status_ & (static_cast<std::uint8_t>(status) << 4)) != 0;
    }
private:
    std::uint8_t status_ = static_cast<std::uint8_t>(Status::None);
};