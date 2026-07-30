#pragma once

#include <array>
#include <vector>
#include <cstdint>
#include <optional>

#include "types.hpp"
#include "message.hpp"

template<typename MessagePayload>
class MessageBox{
public:
    using MessageType = Message<MessagePayload>;
    using MsgBuffer = std::vector<MessageType>;

    MessageBox() = default;
    MessageBox(const MessageBox&) = delete;
    MessageBox(MessageBox&&) noexcept = default;
    MessageBox& operator=(const MessageBox&) = delete;
    MessageBox& operator=(MessageBox&&) noexcept = delete;

    [[nodiscard]] std::size_t addNeighborBox(){
        message_box_[0].emplace_back();
        message_box_[1].emplace_back();

        return message_box_[1].size() - 1; 
    }

    void writeMessage(MessageType&& msg, std::size_t index){
        MsgBuffer& target_buffer = message_box_[write_slot_][index];
        target_buffer.emplace_back(std::move(msg));
    }

    [[nodiscard]] std::optional<MessageType> readMessage(){
        if(buffer_read_index_ == message_box_[read_slot_].size())
            return std::nullopt;

        MsgBuffer& target_buffer = getNextNonEmptyReadBuffer();
        if(buffer_read_index_ == message_box_[read_slot_].size())
            return std::nullopt;


        // TODO change to read messages in order instead of like a stack?
        // does it matter? since its using synchronized phases
        // meaning it assumes order of messages received does not matter in each phase
        MessageType result = std::move(target_buffer.back());
        target_buffer.pop_back();
        return result;
    }

    [[nodiscard]] bool empty() const{
        if(buffer_read_index_ == message_box_[read_slot_].size())
            return true;

        for(const MsgBuffer& buffer : message_box_[read_slot_]){
            if(!buffer.empty())
                return false;
        }

        return true;
    }

    void changePhase(){
        swapBufferPhase();
        buffer_read_index_ = 0;
    }
private:
    enum BufferIndex : std::uint8_t {
        Ping = 0,
        Pong = 1
    };

    std::array<std::vector<MsgBuffer>, 2> message_box_{};
    std::size_t buffer_read_index_{};
    BufferIndex read_slot_ = Ping;
    BufferIndex write_slot_ = Pong;

    void swapBufferPhase(){
        std::swap(read_slot_, write_slot_);
    }

    MsgBuffer& getNextNonEmptyReadBuffer() {
        MsgBuffer& target_buffer = message_box_[read_slot_][buffer_read_index_];
        while(target_buffer.empty()){
            ++buffer_read_index_;
            if(buffer_read_index_ == message_box_[read_slot_].size())
                break;

            target_buffer = message_box_[read_slot_][buffer_read_index_];
        }

        return target_buffer;
    }
};