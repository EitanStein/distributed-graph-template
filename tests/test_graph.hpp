#pragma once

#include "core/graph.hpp"
#include "test_node.hpp"

class TestGraph : public BaseGraph<TestGraph, TestNode, TestNode::Task>{
    using BaseGraph<TestGraph, TestNode, TestNode::Task>::BaseGraph;
    using ThreadTask = TestNode::Task;

    void mainCycleImpl(){
        for(auto& node : nodes_){
            if(!node.isRunningCycle())
                continue;
            thread_pool_.addTask(ThreadTask{node});
        }
    }
};