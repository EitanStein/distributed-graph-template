#define CATCH_CONFIG_MAIN

#include <catch2/catch_all.hpp>
#include <catch2/catch_test_macros.hpp>
#include "core/node.hpp"


struct TestNode{
    const MessageBox& getMessageBox(Node& node) const {return node.message_box;}
    const std::unordered_map<node_id_t, Node::Neighbor> & getNeighbors(Node& node) const {return node.neighbors;}
};

TEST_CASE("sanity check", ""){
    REQUIRE(1==1);
}

