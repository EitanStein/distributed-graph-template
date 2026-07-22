#pragma once

#include <vector>

#include "node.hpp"
#include "utils/thread_pool.hpp"


class Graph{
private:
    std::vector<Node> nodes{};
    ThreadPool<Node::Task> thread_pool;
public:
    Graph(node_id_t size, std::size_t thread_pool_size = std::thread::hardware_concurrency()) : thread_pool(thread_pool_size){
        nodes.reserve(size);
        for(node_id_t id = 0; id < size ; ++id){
            nodes.emplace_back(id);
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
        addEdge(nodes[node1], nodes[node2]);
    }
    
    const std::vector<Node>& getNodes() const {
        return nodes;
    }

    const Node& getNode(node_id_t id) const {
        return nodes.at(id);
    }
    
    Node& getNode(node_id_t id) {
        return nodes[id];
    }
    
    void cycle(){

    }
};