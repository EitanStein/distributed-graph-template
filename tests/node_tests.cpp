#define CATCH_CONFIG_MAIN

#include <catch2/catch_all.hpp>
#include <catch2/catch_test_macros.hpp>

#include "core/node.hpp"
#include "test_node.hpp"

TEST_CASE("single node", ""){
    Node node{1};
    REQUIRE(node.emptyInbox());
    REQUIRE(node.ID() == 1);
    REQUIRE(node.readMessage() == std::nullopt);
    node.postCycle();
    REQUIRE(node.readMessage() == std::nullopt);
}

TEST_CASE("basic neighbor checks", ""){
    Node node1{0};
    Node node2{1};
    Node node3{2};

    node1.addNeighbor(node2);
    node2.addNeighbor(node1);

    TestNode helper;
    REQUIRE(helper.getNeighbors(node1).contains(node2.ID()));
    REQUIRE(helper.getNeighbors(node2).contains(node1.ID()));
    REQUIRE(node1.emptyInbox());
    REQUIRE(node2.emptyInbox());
}

TEST_CASE("double neighbor add", ""){
    Node node1{0};
    Node node2{1};

    node1.addNeighbor(node2);

    TestNode helper;
    REQUIRE(helper.getNeighbors(node1).size() == 1);

    node1.addNeighbor(node2);
    REQUIRE(helper.getNeighbors(node1).size() == 1);
}

TEST_CASE("sending messages", ""){
    Node node1{0};
    Node node2{1};

    node1.addNeighbor(node2);
    node2.addNeighbor(node1);

    node1.sendMessage(node2.ID(), msg_t{});
    REQUIRE(node2.emptyInbox());

    node2.postCycle();
    REQUIRE(!node2.emptyInbox());
    REQUIRE(node2.readMessage().value().payload == msg_t{});
    REQUIRE(node2.emptyInbox());

    Node node3{2};
    node2.addNeighbor(node3);
    node3.addNeighbor(node2);

    node1.broadcast(msg_t{});
    REQUIRE(node2.emptyInbox());
    REQUIRE(node3.emptyInbox());

    node2.postCycle();
    node3.postCycle();

    REQUIRE(!node2.emptyInbox());
    REQUIRE(node2.readMessage().value().payload == msg_t{});
    REQUIRE(node2.emptyInbox());
    REQUIRE(node3.emptyInbox());
}

