//
// Created by Alejandro Fernández on 06/11/2023.
//

#ifndef PARALLEL_BFS_PARALLEL_BFS_TASKS_H
#define PARALLEL_BFS_PARALLEL_BFS_TASKS_H

#include <queue>
#include <future>
#include <thread>
#include "../problem.h"
#include "../node.h"

namespace parallel_bfs {
    /// In order to avoid data races, ParallelBFSTasks only works with tree-like search.
    class ParallelBFSTasks { // FIXME: Change this from Breadth First Search to Best First Search
    public:
        template<Searchable State, std::derived_from<BaseTransitionModel<State>> TM>
        [[nodiscard]] std::shared_ptr<Node<State>> operator()(const Problem<State, TM> &problem) const {
            auto init_node = std::make_shared<Node<State>>(problem.initial());
            // if (problem.is_goal(init_node->state())) return init_node;
            //
            // for (const auto &child: problem.expand(init_node)) {
            //
            // }
            auto lambda = [this, init_node, &problem]() { return this->_search(init_node, problem); };
            std::packaged_task task(lambda);
            std::future<std::shared_ptr<Node<State>>> future {task.get_future()};
            std::jthread thread{std::move(task)};
            return future.get();
        }

    private:
        template<typename State, typename TM>
        [[nodiscard]] std::shared_ptr<Node<State>> _search(std::shared_ptr<Node<State>> init_node, const Problem<State, TM> &problem) const {
            if (problem.is_goal(init_node->state())) return init_node;
            std::queue<std::shared_ptr<Node<State>>> frontier({std::move(init_node)});

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
#endif //PARALLEL_BFS_PARALLEL_BFS_TASKS_H
