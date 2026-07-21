#pragma once

#include <vector>

#include "node.hpp"
#include "utils/thread_pool.hpp"


class Graph{
private:
    std::vector<Node> nodes{};
    ThreadPool<Node::Task> thread_pool;
};