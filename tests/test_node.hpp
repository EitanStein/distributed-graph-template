#pragma once

#include "core/node.hpp"

struct TestNode{
    const MessageBox& getMessageBox(Node& node) const {return node.message_box;}
    const std::unordered_map<node_id_t, Node::Neighbor>& getNeighbors(Node& node) const {return node.neighbors;}
};