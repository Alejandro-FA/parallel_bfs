//
// Created by Alejandro Fernández on 06/11/2023.
//

#ifndef PARALLEL_BFS_PARALLEL_BFS_H
#define PARALLEL_BFS_PARALLEL_BFS_H

#include <queue>
#include "problem.h"
#include "node.h"

namespace parallel_bfs {
    /// In order to avoid data races, ParallelBFS only works with tree-like search.
    class ParallelBFS { // FIXME: Change this from Breadth First Search to Best First Search
    public:
        template<typename State, typename TM>
        [[nodiscard]] std::shared_ptr<Node<State>> operator()(const Problem<State, TM> &problem) const {
            auto init_node = std::make_shared<Node<State>>(problem.initial());
            if (problem.is_goal(problem.initial())) return init_node;
            std::queue<std::shared_ptr<Node<State>>> frontier({init_node});

            while (!frontier.empty()) {
                auto node = frontier.front();
                frontier.pop();
                for (const auto &child: problem.expand(node)) {
                    State child_state = child->state();
                    if (problem.is_goal(child_state)) return child;
                    frontier.push(child);
                }
            }
            return nullptr;
        }
    };
}
#endif //PARALLEL_BFS_PARALLEL_BFS_H
