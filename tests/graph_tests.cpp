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
