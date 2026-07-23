#define CATCH_CONFIG_MAIN

#include <catch2/catch_all.hpp>
#include <catch2/catch_test_macros.hpp>

#include "core/graph.hpp"
#include "test_node.hpp"

TEST_CASE("single node", ""){
    Graph g(1);
    REQUIRE(g.getNodes().size() == 1);
    REQUIRE(g.getNodes().at(0).ID() == 0);
}


TEST_CASE("multiple nodes", ""){
    Graph g(2);
    REQUIRE(g.getNodes().size() == 2);
    REQUIRE(g.getNodes().at(0).ID() == 0);
    REQUIRE(g.getNodes().at(1).ID() == 1);

    g.addEdge(0, 1);
    TestNode helper;
    const auto& neighbors = helper.getNeighbors(g.getNode(1));
    REQUIRE(neighbors.size() == 1);
    REQUIRE(neighbors.contains(0));
}
