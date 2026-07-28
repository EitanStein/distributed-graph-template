#pragma once

#include <utility>
#include <unordered_map>
#include <ranges>
#include <concepts>

#include "message_box.hpp"
#include "types.hpp"
#include "utils/log_macros.hpp"



template<typename Derived, typename MsgArg>
concept ValidDerived = requires(Derived derived, MsgArg&& arg){
    {derived.handleMessage(std::forward<MsgArg>(arg))} -> std::same_as<void>;
    {derived.preCycleImpl()} -> std::same_as<void>;
};

template<typename Derived>
class BaseNode{
protected:
    struct Neighbor{
        std::reference_wrapper<Derived> neighbor;
        std::size_t neighbor_mbox_index;

        Neighbor(Derived& neighbor, std::size_t neighbor_mbox_index) : neighbor(neighbor), neighbor_mbox_index(neighbor_mbox_index) {}
    };
    

    const node_id_t id{};
    std::unordered_map<node_id_t, Neighbor> neighbors{};
private:
    
    MessageBox message_box{};

    void receiveMessage(Message&& msg){
        const std::size_t neighbor_mbox_index = neighbors.at(msg.sender).neighbor_mbox_index;
        message_box.writeMessage(std::move(msg), neighbor_mbox_index);
    }
    
    void preCycle() requires ValidDerived<Derived, Message> {
        static_case<Derived*>(this)->preCycleImpl();
    }
public:
    BaseNode(node_id_t id) : id(id) {
        static_assert(std::derived_from<Derived, BaseNode>);
    }

    BaseNode(const BaseNode&) = delete;
    BaseNode(BaseNode&&) noexcept = default;

    BaseNode& operator=(const BaseNode&) = delete;
    BaseNode& operator=(BaseNode&&) noexcept = delete;

    void addNeighbor(Derived& neighbor){
        const std::size_t neighbor_mbox_index = message_box.addNeighborBox();
        neighbors.emplace(std::piecewise_construct, std::forward_as_tuple(neighbor.ID()), std::forward_as_tuple(neighbor, neighbor_mbox_index));
    }

    [[nodiscard]] node_id_t ID() const noexcept {return id;}

    [[nodiscard]] bool emptyInbox() const {return message_box.empty();}

    [[nodiscard]] std::optional<Message> readMessage(){
        return message_box.readMessage();
    }

    void handleAllInobxMessages() requires ValidDerived<Derived, Message> {
        while(std::optional<Message> msg = readMessage()){
            static_cast<Derived*>(this)->handleMessage(std::move(msg.value()));
        }
    }

    void sendMessage(node_id_t neighbor_id, msg_t&& msg){
        if(!neighbors.contains(neighbor_id)){
            LOG_ERROR("node {} does not contain a neighbor with id {}", id, neighbor_id);
            return;
        }
            
        Neighbor& neighbor = neighbors.at(neighbor_id);
        neighbor.neighbor.get().receiveMessage(Message(id, neighbor.neighbor.get().ID(), std::move(msg)));
    }

    void broadcast(msg_t&& msg){
        for(Neighbor& neighbor : std::views::values(neighbors)){
            neighbor.neighbor.get().receiveMessage(Message(id, neighbor.neighbor.get().ID(), msg));
        }
    }

    void cycle(){
        handleAllInobxMessages();
    }

    void postCycle() {
        message_box.changePhase();
    }

    struct Task{
        std::reference_wrapper<Derived> node;

        Task(Derived& node) : node(node) {}

        void operator()() {
            node.get().preCycle();
            node.get().cycle();
        }
    };

};


