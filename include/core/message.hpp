#pragma once

#include "types.hpp"

struct Message{
    node_id_t sender;
    node_id_t recipient;
    msg_t payload;

    explicit Message(node_id_t sender, node_id_t recipient, const msg_t& msg) : 
                    sender(sender), recipient(recipient), payload(msg) {}

    explicit Message(node_id_t sender, node_id_t recipient, msg_t&& msg) : 
                    sender(sender), recipient(recipient), payload(std::move(msg)) {}
};