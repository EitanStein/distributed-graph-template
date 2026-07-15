#pragma once
#include "types.hpp"
#include <memory>

class Node;

struct Message{
    std::reference_wrapper<const Node> sender;
    std::reference_wrapper<const Node> recipient;
    msg_t payload;

    explicit Message(const Node& sender, const Node& recipient, const msg_t& msg) : 
                    sender(sender), recipient(recipient), payload(msg) {}

    explicit Message(const Node& sender, const Node& recipient, msg_t&& msg) : 
                    sender(sender), recipient(recipient), payload(std::move(msg)) {}
};