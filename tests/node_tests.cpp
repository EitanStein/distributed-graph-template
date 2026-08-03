#define CATCH_CONFIG_MAIN

#include <catch2/catch_all.hpp>
#include <catch2/catch_test_macros.hpp>

#include "core/node.hpp"
#include "test_node.hpp"

TEST_CASE("single node", ""){
    TestNode node{1};
    REQUIRE(node.emptyInbox());
    REQUIRE(node.ID() == 1);
    REQUIRE(node.readMessage() == std::nullopt);
    node.postCycle();
    REQUIRE(node.readMessage() == std::nullopt);
}

TEST_CASE("basic neighbor checks", ""){
    TestNode node1{0};
    TestNode node2{1};
    TestNode node3{2};

    node1.addNeighbor(node2);
    node2.addNeighbor(node1);

    REQUIRE(node1.isNeighbor(node2.ID()));
    REQUIRE(node2.isNeighbor(node1.ID()));
    REQUIRE(node1.emptyInbox());
    REQUIRE(node2.emptyInbox());
}

TEST_CASE("double neighbor add", ""){
    TestNode node1{0};
    TestNode node2{1};

    node1.addNeighbor(node2);

    REQUIRE(node1.getNumNeighbors() == 1);

    node1.addNeighbor(node2);
    REQUIRE(node1.getNumNeighbors() == 1);
}

TEST_CASE("sending messages", ""){
    TestNode node1{0};
    TestNode node2{1};

    node1.addNeighbor(node2);
    node2.addNeighbor(node1);

    node1.sendMessage(node2.ID(), TestNode::MessagePayload{});
    REQUIRE(node2.emptyInbox());

    node2.postCycle();
    REQUIRE(!node2.emptyInbox());
    REQUIRE(node2.readMessage().value().payload_ == TestNode::MessagePayload{});
    REQUIRE(node2.emptyInbox());

    TestNode node3{2};
    node2.addNeighbor(node3);
    node3.addNeighbor(node2);

    node1.broadcast(TestNode::MessagePayload{});
    REQUIRE(node2.emptyInbox());
    REQUIRE(node3.emptyInbox());

    node2.postCycle();
    node3.postCycle();

    REQUIRE(!node2.emptyInbox());
    REQUIRE(node2.readMessage().value().payload_ == TestNode::MessagePayload{});
    REQUIRE(node2.emptyInbox());
    REQUIRE(node3.emptyInbox());
}

TEST_CASE("node status changes", ""){
    TestNode node1{0};
    TestNode node2{1};

    node1.addNeighbor(node2);
    node2.addNeighbor(node1);

    REQUIRE(!node1.isRunningCycle());
    REQUIRE(!node2.isRunningCycle());

    node1.sendMessage(1, TestNode::MessagePayload{});
    REQUIRE(!node1.receivedMsgLastCycle());
    REQUIRE(!node1.sentMsgLastCycle());
    REQUIRE(!node1.isRunningCycle());
    REQUIRE(!node2.isRunningCycle());
    REQUIRE(!node2.receivedMsgLastCycle());

    node1.postCycle();
    node2.postCycle();

    REQUIRE(!node1.receivedMsgLastCycle());
    REQUIRE(node1.sentMsgLastCycle());
    REQUIRE(!node1.isRunningCycle());
    REQUIRE(node2.isRunningCycle());
    REQUIRE(node2.receivedMsgLastCycle());

    node2.cycle();
    node1.postCycle();
    node2.postCycle();

    REQUIRE(!node1.receivedMsgLastCycle());
    REQUIRE(!node1.sentMsgLastCycle());
    REQUIRE(!node1.isRunningCycle());
    REQUIRE(!node2.isRunningCycle());
    REQUIRE(!node2.receivedMsgLastCycle());
}