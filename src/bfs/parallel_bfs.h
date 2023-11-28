//
// Created by Alejandro Fernández on 06/11/2023.
//

#ifndef PARALLEL_BFS_PARALLEL_BFS_H
#define PARALLEL_BFS_PARALLEL_BFS_H

#include <queue>

class ParallelBFS {
public:
    template<typename T>
    [[nodiscard]] std::shared_ptr<Node<T>> operator()(const Problem<T> &problem) const {
        auto init_node = std::make_shared<Node<T>>(problem.initial());
        std::queue<std::shared_ptr<Node<T>>> frontier({init_node});
        std::unordered_set<T> reached({init_node->state()}); // We use a set instead of a map because all actions have the same cost.

        while (!frontier.empty()) {
            auto node = frontier.front();
            frontier.pop();
            auto children = problem.expand(node);
            for (const auto &child : children) {
                T child_state = child->state();
                if (problem.is_goal(child_state)) return child;
                if (!reached.contains(child_state)) {
                    reached.insert(child_state);
                    frontier.push(child);
                }
            }
        }
        return nullptr;
    }
};

#endif //PARALLEL_BFS_PARALLEL_BFS_H
