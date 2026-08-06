#pragma once

#include <utility>
#include <unordered_map>
#include <ranges>
#include <concepts>

#include "message_box.hpp"
#include "types.hpp"
#include "utils/log_macros.hpp"
#include "node_status.hpp"


template<typename Derived, typename MsgArg>
concept ValidDerivedNode = requires(Derived& derived, MsgArg&& arg){
    {derived.handleMessage(std::forward<MsgArg>(arg))} -> std::same_as<void>;
    {derived.preCycleImpl()} -> std::same_as<void>;
};

template<typename Derived, typename MsgPayload>
class BaseNode{
public:
    using MessagePayload = MsgPayload;
    using MessageType = MessageBox<MessagePayload>::MessageType;

    struct Task{
        std::reference_wrapper<Derived> node;

        Task(Derived& node) : node(node) {}
        Task(const Task&) = default;
        Task(Task&&) noexcept = default;
        Task& operator=(const Task&) = default;
        Task& operator=(Task&&) noexcept = default;

        void operator()() {
            node.get().preCycle();
            node.get().cycle();
        }
    };

    BaseNode(node_id_t id) : id_(id) {
        static_assert(std::derived_from<Derived, BaseNode>);
    }

    BaseNode(const BaseNode&) = delete;
    BaseNode(BaseNode&&) noexcept = default;

    BaseNode& operator=(const BaseNode&) = delete;
    BaseNode& operator=(BaseNode&&) noexcept = delete;

    void addNeighbor(Derived& neighbor){
        const std::size_t neighbor_mbox_index = message_box_.addNeighborBox();
        neighbors_.emplace(std::piecewise_construct, std::forward_as_tuple(neighbor.ID()), std::forward_as_tuple(neighbor, neighbor_mbox_index));
    }

    [[nodiscard]] node_id_t ID() const noexcept {return id_;}

    [[nodiscard]] bool emptyInbox() const {return message_box_.empty();}

    [[nodiscard]] std::optional<MessageType> readMessage(){
        return message_box_.readMessage();
    }

    void handleAllInobxMessages() requires ValidDerivedNode<Derived, MessageType> {
        while(std::optional<MessageType> msg = readMessage()){
            static_cast<Derived*>(this)->handleMessage(std::move(msg.value()));
        }
    }

    void sendMessage(node_id_t neighbor_id, MessagePayload&& msg){
        if(!neighbors_.contains(neighbor_id)){
            LOG_ERROR("node {} does not contain a neighbor with id {}", id_, neighbor_id);
            return;
        }
            
        Derived& neighbor_node = neighbors_.at(neighbor_id).neighbor_.get();
        neighbor_node.receiveMessage(MessageType(id_, neighbor_node.ID(), std::move(msg)));
        status_.updateStatus(NodeStatus::Status::SentMsg);
    }

    void broadcast(MessagePayload&& msg){
        for(Neighbor& neighbor : std::views::values(neighbors_)){
            Derived& neighbor_node = neighbor.neighbor_.get();
            neighbor_node.receiveMessage(MessageType(id_, neighbor_node.ID(), msg));
        }
        status_.updateStatus(NodeStatus::Status::SentMsg);
    }

    void cycle(){
        handleAllInobxMessages();
    }

    void postCycle() {
        status_.cycleStatus();
        message_box_.changePhase();
    }

    [[nodiscard]] bool receivedMsgLastCycle() const noexcept {
        return status_.isLastCycleStatus(NodeStatus::Status::ReceivedMsg);
    }

    [[nodiscard]] bool sentMsgLastCycle() const noexcept {
        return status_.isLastCycleStatus(NodeStatus::Status::SentMsg);
    }

    [[nodiscard]] bool isRunningCycle() const noexcept {
        return status_.isLastCycleStatus(NodeStatus::Status::ReceivedMsg) || status_.isLastCycleStatus(NodeStatus::Status::Init);
    }
protected:
    struct Neighbor{
        std::reference_wrapper<Derived> neighbor_;
        std::size_t neighbor_mbox_index_;

        Neighbor(Derived& neighbor, std::size_t neighbor_mbox_index) : neighbor_(neighbor), neighbor_mbox_index_(neighbor_mbox_index) {}
    };
    using NeighborMap = std::unordered_map<node_id_t, Neighbor>;

    const node_id_t id_{};
    NeighborMap neighbors_{};
    NodeStatus status_{};

    void receiveMessage(MessageType&& msg){
        const std::size_t neighbor_mbox_index = neighbors_.at(msg.sender_).neighbor_mbox_index_;
        message_box_.writeMessage(std::move(msg), neighbor_mbox_index);
        status_.updateStatus(NodeStatus::Status::ReceivedMsg);
    }
private:
    MessageBox<MessagePayload> message_box_{};
    
    void preCycle() requires ValidDerivedNode<Derived, MessageType> {
        static_cast<Derived*>(this)->preCycleImpl();
    } 
};


template<typename DerivedNode>
concept ValidNode = std::derived_from<DerivedNode, BaseNode<DerivedNode, typename DerivedNode::MessagePayload>>;

