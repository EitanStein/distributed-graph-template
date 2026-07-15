#pragma once
#include "types.hpp"
#include <utility>
#include <unordered_map>
#include "message_box.hpp"


class Node{
private:
    struct Neighbor{
        std::reference_wrapper<Node> neighbor;
        std::size_t neighbor_mbox_index;
    };

    node_id_t id{};
    std::unordered_map<node_id_t, Neighbor> neighbors{};
    MessageBox message_box{};
public:
    explicit Node(node_id_t id) : id(id) {};
    
    Node(const Node&) = delete;
    Node& operator=(const Node&) = delete;

    Node(Node&&) noexcept = default;
    Node& operator=(Node&&) noexcept = default;
};