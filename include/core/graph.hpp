#pragma once

#include <vector>
#include <concepts>

#include "node.hpp"
#include "utils/thread_pool.hpp"


template<typename Derived>
concept ValidDerivedGraph = requires(Derived& graph){
    {graph.mainCycleImpl()} -> std::same_as<void>;
};


template<typename Derived, ValidNode Node, std::invocable Task>
class BaseGraph{
public:
    BaseGraph(node_id_t size, std::size_t thread_pool_size = ThreadPoolInfo::default_thread_count) : thread_pool_(thread_pool_size){
        static_assert(std::derived_from<Derived, BaseGraph>);

        nodes_.reserve(size);
        for(node_id_t id = 0; id < size ; ++id){
            nodes_.emplace_back(id);
        }
    }

    BaseGraph(const BaseGraph&) = delete;
    BaseGraph(BaseGraph&&) noexcept = delete;
    BaseGraph& operator=(const BaseGraph&) = delete;
    BaseGraph& operator=(BaseGraph&&) noexcept = delete;

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
protected:
    ThreadPool<Task> thread_pool_;
    std::vector<Node> nodes_{};

    void mainCycle() requires ValidDerivedGraph<Derived> {
        static_cast<Derived*>(this)->mainCycleImpl();
    }

    void postCycle() {
        for(auto& node : nodes_){
            node.postCycle();
        }
    }
};