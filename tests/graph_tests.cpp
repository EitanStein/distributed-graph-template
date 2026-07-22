#define CATCH_CONFIG_MAIN

#include <catch2/catch_all.hpp>
#include <catch2/catch_test_macros.hpp>
#include "core/graph.hpp"

struct TestNode{
    const MessageBox& getMessageBox(Node& node) const {return node.message_box;}
    const std::unordered_map<node_id_t, Node::Neighbor>& getNeighbors(Node& node) const {return node.neighbors;}
};

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
