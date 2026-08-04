#pragma once

#include "core/node.hpp"

class TestNode : public BaseNode<TestNode, int>{
public:
    TestNode(node_id_t id) : BaseNode(id) {}

    [[nodiscard]] bool isNeighbor(node_id_t id) const {return neighbors_.contains(id);}
    [[nodiscard]] std::size_t getNumNeighbors() const {return neighbors_.size();}

    void handleMessage(MessageType&&) {}
    void preCycleImpl() {}
};