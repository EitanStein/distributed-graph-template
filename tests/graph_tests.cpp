#define CATCH_CONFIG_MAIN

#include <catch2/catch_all.hpp>
#include <catch2/catch_test_macros.hpp>

#include "test_graph.hpp"
#include "test_node.hpp"

TEST_CASE("single node", ""){
    TestGraph g(1);
    REQUIRE(g.getNodes().size() == 1);
    REQUIRE(g.getNodes().at(0).ID() == 0);
}


TEST_CASE("multiple nodes", ""){
    TestGraph g(2);
    REQUIRE(g.getNodes().size() == 2);
    REQUIRE(g.getNodes().at(0).ID() == 0);
    REQUIRE(g.getNodes().at(1).ID() == 1);

    g.addEdge(0, 1);
    REQUIRE(g.getNodes().at(0).getNumNeighbors() == 1);
    REQUIRE(g.getNodes().at(0).isNeighbor(1));
}

TEST_CASE("sending messages", ""){
    TestGraph g(2);
    g.addEdge(0, 1);

    g.getNode(0).broadcast(TestNode::MessagePayload{5});
    REQUIRE(!g.getNode(1).isRunningCycle());
    REQUIRE(!g.getNode(1).receivedMsgLastCycle());
    REQUIRE(!g.getNode(0).sentMsgLastCycle());
    REQUIRE(!g.getNode(0).isRunningCycle());

    g.cycle();

    REQUIRE(g.getNode(1).isRunningCycle());
    REQUIRE(g.getNode(1).receivedMsgLastCycle());
    REQUIRE(g.getNode(0).sentMsgLastCycle());
    REQUIRE(!g.getNode(0).isRunningCycle());

    g.cycle();

    REQUIRE(!g.getNode(1).isRunningCycle());
    REQUIRE(!g.getNode(1).receivedMsgLastCycle());
    REQUIRE(!g.getNode(0).sentMsgLastCycle());
    REQUIRE(!g.getNode(0).isRunningCycle());
}
