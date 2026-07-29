#pragma once

#include "types.hpp"

template<typename Payload>
struct Message{
    node_id_t sender;
    node_id_t recipient;
    Payload payload;

    explicit Message(node_id_t sender, node_id_t recipient, const Payload& msg) : 
                    sender(sender), recipient(recipient), payload(msg) {}

    explicit Message(node_id_t sender, node_id_t recipient, Payload&& msg) : 
                    sender(sender), recipient(recipient), payload(std::move(msg)) {}
};