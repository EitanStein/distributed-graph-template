#pragma once

#include "core/node.hpp"

class TestNode : public BaseNode<TestNode>{
public:
    TestNode(node_id_t id) : BaseNode(id) {}

    bool isNeighbor(node_id_t id) const {return neighbors.contains(id);}
    std::size_t getNumNeighbors() const {return neighbors.size();}

    void handleMessage(Message&&) {}
    void preCycleImpl() {}
};