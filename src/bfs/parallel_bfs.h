//
// Created by Alejandro Fernández on 06/11/2023.
//

#ifndef PARALLEL_BFS_PARALLEL_BFS_H
#define PARALLEL_BFS_PARALLEL_BFS_H

#include <queue>
#include <unordered_set>
#include <algorithm>
#include "../node.h"
#include "../problem.h"
#include "../state.h"
#include "base_bfs.h"

class ParallelBFS final : public BFS {
public:
    [[nodiscard]] std::shared_ptr<Node> operator()(const Problem &problem) const override {
        std::shared_ptr<Node> init_node = std::make_shared<Node>(problem.initial());
        std::queue<std::shared_ptr<Node>> frontier({init_node});
        std::unordered_set<State> reached({init_node->state()}); // We use a set instead of a map because all actions have the same cost.

        while (!frontier.empty()) {
            auto node = frontier.front();
            frontier.pop();
            if (problem.is_goal(node->state())) return node; // We do not use early-goal optimization to mimic general Best First Search
            auto children = problem.expand(node);
            std::for_each(children.cbegin(), children.cend(),
                          [&frontier, &reached](auto node) { _reach_node(node, frontier, reached); });
        }
        return nullptr;
    }

    [[nodiscard]] std::string get_name() const override { return _name; }

private:
    inline static const std::string _name{"Synchronous BFS"};

    void static _reach_node(const std::shared_ptr<Node> &node, std::queue<std::shared_ptr<Node>> &frontier, std::unordered_set<State> &reached) {
        State s = node->state();
        if (!reached.contains(s)) {
            reached.insert(s);
            frontier.push(node);
        }
    }
};

#endif //PARALLEL_BFS_PARALLEL_BFS_H
