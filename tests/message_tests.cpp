#define CATCH_CONFIG_MAIN

#include <catch2/catch_all.hpp>
#include <catch2/catch_test_macros.hpp>

#include "core/message_box.hpp"
#include "core/node.hpp"
#include "test_node.hpp"

TEST_CASE("empty message box test", ""){
    MessageBox message_box;

    REQUIRE(message_box.empty());
    REQUIRE(message_box.readMessage() == std::nullopt);
}

TEST_CASE("test writing and reading message", ""){
    TestNode node1{0};
    TestNode node2{1};
    MessageBox message_box;

    REQUIRE(message_box.addNeighborBox() == 0);
    REQUIRE(message_box.addNeighborBox() == 1);

    msg_t msg;
    message_box.writeMessage(Message(node1.ID(), node2.ID(), msg), 0);
    REQUIRE(message_box.empty());
    REQUIRE(message_box.readMessage() == std::nullopt);

    message_box.changePhase();
    REQUIRE(!message_box.empty());
    REQUIRE(message_box.readMessage().value().payload == msg);

    REQUIRE(message_box.empty());
    REQUIRE(message_box.readMessage() == std::nullopt);
}

