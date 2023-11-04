//
// Created by Alejandro Fernández on 04/11/2023.
//

#ifndef PARALLEL_BFS_BFS_H
#define PARALLEL_BFS_BFS_H

#include <memory>
#include <queue>
#include <unordered_set>
#include "node.h"
#include "problem.h"
#include "state.h"

std::shared_ptr<Node> breadth_first_search(const Problem &problem) {
    std::shared_ptr<Node> init_node = std::make_shared<Node>(problem.initial());

    if (problem.is_goal(init_node->state())) return init_node;
    std::queue<std::shared_ptr<Node>> frontier({init_node});
    std::unordered_set<State> reached{init_node->state()};

    while (!frontier.empty()) {
        auto node = frontier.front();
        frontier.pop();
        auto children = problem.expand(node);
        for (const auto &child : children) {
            State s = child->state();
            if (problem.is_goal(s)) return child;
            if (!reached.contains(s)) {
                reached.insert(s);
                frontier.push(child);
            }
        }
    }
    return nullptr;
}

#endif //PARALLEL_BFS_BFS_H
