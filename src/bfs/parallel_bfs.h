//
// Created by Alejandro Fernández on 06/11/2023.
//

#ifndef PARALLEL_BFS_PARALLEL_BFS_H
#define PARALLEL_BFS_PARALLEL_BFS_H

#include <queue>

/// In order to avoid data races, ParallelBFS only works with tree-like search.
class ParallelBFS {
public:
    template<typename T>
    [[nodiscard]] std::shared_ptr<Node<T>> operator()(const Problem<T> &problem) const {
        auto init_node = std::make_shared<Node<T>>(problem.initial());
        std::queue<std::shared_ptr<Node<T>>> frontier({init_node});

        while (!frontier.empty()) {
            auto node = frontier.front();
            frontier.pop();
            auto children = problem.expand(node);
            for (const auto &child: children) {
                T child_state = child->state();
                if (problem.is_goal(child_state)) return child;
                frontier.push(child);
            }
        }
        return nullptr;
    }
};

#endif //PARALLEL_BFS_PARALLEL_BFS_H
