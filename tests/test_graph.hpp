#pragma once

#include "core/graph.hpp"
#include "test_node.hpp"

class TestGraph : public BaseGraph<TestNode>{
public:
    using BaseGraph<TestNode>::BaseGraph;
    ~TestGraph() = default;
protected:
    void mainCycle() override{
        for(auto& node : nodes_){
            if(!node.isRunningCycle())
                continue;
            thread_pool_.addTask(Task{node});
        }
    }
};