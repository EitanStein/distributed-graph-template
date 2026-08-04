#pragma once

#include <vector>
#include <concepts>

#include "node.hpp"
#include "utils/thread_pool.hpp"


template<ValidNode Node>
class Graph{
public:
    Graph(node_id_t size, std::size_t thread_pool_size = ThreadInfo::default_thread_count) : thread_pool_(thread_pool_size){
        nodes_.reserve(size);
        for(node_id_t id = 0; id < size ; ++id){
            nodes_.emplace_back(id);
        }
    }

    Graph(const Graph&) = delete;
    Graph(Graph&&) noexcept = delete;
    Graph& operator=(const Graph&) = delete;
    Graph& operator=(Graph&&) noexcept = delete;

    void addEdge(Node& node1, Node& node2){
        node1.addNeighbor(node2);
        node2.addNeighbor(node1);
    }

    void addEdge(node_id_t node1, node_id_t node2){
        addEdge(nodes_[node1], nodes_[node2]);
    }
    
    [[nodiscard]] const std::vector<Node>& getNodes() const noexcept{
        return nodes_;
    }

    [[nodiscard]] const Node& getNode(node_id_t id) const {
        return nodes_.at(id);
    }
    
    [[nodiscard]] Node& getNode(node_id_t id) {
        return nodes_[id];
    }
    
    void cycle(){
        mainCycle();
        thread_pool_.waitForEmptyQueue();
        
        // assumes post cycle is a lightweight operation - does not use threads
        postCycle();
    }
private:
    using ThreadTask = Node::Task;

    ThreadPool<ThreadTask> thread_pool_;
    std::vector<Node> nodes_{};

    void mainCycle(){
        // TODO consider batching (active) nodes for each task so the task queue is smaller 
        // and less time is spent on lock contention when adding/extracting tasks
        for(auto& node : nodes_){
            if(!node.isRunningCycle())
                continue;
            thread_pool_.addTask(ThreadTask{node});
        }
    }

    void postCycle(){
        for(auto& node : nodes_){
            node.postCycle();
        }
    }
};