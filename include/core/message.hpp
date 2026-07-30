#pragma once

#include "types.hpp"

template<typename Payload>
struct Message{
    node_id_t sender_;
    node_id_t recipient_;
    Payload payload_;

    explicit Message(node_id_t sender, node_id_t recipient, const Payload& msg) : 
                    sender_(sender), recipient_(recipient), payload_(msg) {}

    explicit Message(node_id_t sender, node_id_t recipient, Payload&& msg) : 
                    sender_(sender), recipient_(recipient), payload_(std::move(msg)) {}
};