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
private:
    using MsgBuffer = std::vector<MessageType>;

    enum BufferIndex : std::uint8_t {
        Ping = 0,
        Pong = 1
    };

    std::array<std::vector<MsgBuffer>, 2> message_box{};
    std::size_t buffer_read_index{};
    BufferIndex read_slot = Ping;
    BufferIndex write_slot = Pong;

    void swapBufferPhase(){
        std::swap(read_slot, write_slot);
    }

public:
    MessageBox() = default;
    MessageBox(const MessageBox&) = delete;
    MessageBox(MessageBox&&) noexcept = default;
    MessageBox& operator=(const MessageBox&) = delete;
    MessageBox& operator=(MessageBox&&) noexcept = delete;

    [[nodiscard]] std::size_t addNeighborBox(){
        message_box[0].emplace_back();
        message_box[1].emplace_back();

        return message_box[1].size() - 1; 
    }

    void writeMessage(MessageType&& msg, std::size_t index){
        MsgBuffer& target_buffer = message_box[write_slot][index];
        target_buffer.emplace_back(std::move(msg));
    }

    [[nodiscard]] std::optional<MessageType> readMessage(){
        if(buffer_read_index == message_box[read_slot].size())
            return std::nullopt;

        MsgBuffer& target_buffer = message_box[read_slot][buffer_read_index];
        while(target_buffer.empty()){
            ++buffer_read_index;
            if(buffer_read_index == message_box[read_slot].size())
                return std::nullopt;

            target_buffer = message_box[read_slot][buffer_read_index];
        }

        // TODO change to read messages in order instead of like a stack?
        // does it matter? since we are using synchronized phases
        // meaning we assume order of messages received does not matter in each phase
        MessageType result = std::move(target_buffer.back());
        target_buffer.pop_back();
        return result;
    }

    [[nodiscard]] bool empty() const{
        if(buffer_read_index == message_box[read_slot].size())
            return true;

        for(const MsgBuffer& buffer : message_box[read_slot]){
            if(!buffer.empty())
                return false;
        }

        return true;
    }

    void changePhase(){
        swapBufferPhase();
        buffer_read_index = 0;
    }

};