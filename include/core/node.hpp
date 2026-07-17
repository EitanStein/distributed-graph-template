#pragma once
#include "types.hpp"
#include <utility>
#include <unordered_map>
#include <ranges>
#include "message_box.hpp"


class Node{
private:
    struct Neighbor{
        std::reference_wrapper<Node> neighbor;
        std::size_t neighbor_mbox_index;

        Neighbor(Node& neighbor, std::size_t neighbor_mbox_index) : neighbor(neighbor), neighbor_mbox_index(neighbor_mbox_index) {}
    };

    node_id_t id{};
    std::unordered_map<node_id_t, Neighbor> neighbors{};
    MessageBox message_box{};

    friend struct TestNode;

    void receiveMessage(Message&& msg){
        const std::size_t neighbor_mbox_index = neighbors.at(msg.sender.get().ID()).neighbor_mbox_index;
        message_box.writeMessage(std::move(msg), neighbor_mbox_index);
    }

    void handleMessage(Message&& msg);
public:
    explicit Node(node_id_t id) : id(id) {};

    Node(const Node&) = delete;
    Node& operator=(const Node&) = delete;

    Node(Node&&) noexcept = default;
    Node& operator=(Node&&) noexcept = default;

    void addNeighbor(Node& neighbor){
        const std::size_t neighbor_mbox_index = message_box.addNeighborBox();
        neighbors.emplace(std::piecewise_construct, std::forward_as_tuple(neighbor.ID()), std::forward_as_tuple(neighbor, neighbor_mbox_index));
    }

    [[nodiscard]] node_id_t ID() const {return id;}

    [[nodiscard]] bool emptyInbox() const {return message_box.empty();}

    [[nodiscard]] std::optional<Message> readMessage(){
        return message_box.readMessage();
    }

    void handleAllInobxMessages(){
        while(std::optional<Message> msg = readMessage()){
            handleMessage(std::move(msg.value()));
        }
    }

    void sendMessage(node_id_t neighbor_id, msg_t&& msg){
        if(!neighbors.contains(neighbor_id)){
            // TODO log error
            [[maybe_unused]] int x = 0;
            return;
        }
            
        Neighbor& neighbor = neighbors.at(neighbor_id);
        neighbor.neighbor.get().receiveMessage(Message(*this, neighbor.neighbor.get(), std::move(msg)));
    }

    void broadcast(msg_t&& msg){
        for(Neighbor& neighbor : std::views::values(neighbors)){
            neighbor.neighbor.get().receiveMessage(Message(*this, neighbor.neighbor.get(), msg));
        }
    }

    void preCycle();

    void cycle(){
        handleAllInobxMessages();
    }

    void postCycle();

};