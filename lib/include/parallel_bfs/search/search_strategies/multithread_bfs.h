//
// Created by Alejandro Fernández on 06/11/2023.
//

#ifndef PARALLEL_BFS_MULTITHREAD_BFS_H
#define PARALLEL_BFS_MULTITHREAD_BFS_H

#include <vector>
#include <memory>
#include <stop_token>
#include <thread>
#include <iostream>
#include "../problem.h"
#include "bfs.h"


namespace parallel_bfs::detail {
    class ThreadDirector {
        // TODO: implement
    };


    template<Searchable State, std::derived_from<BaseTransitionModel<State>> TM>
    class WorkerThread {
    public:
        void operator()(const Problem<State, TM> &problem, std::stop_source stop_source) {
            const std::stop_token token = stop_source.get_token();

            while (!token.stop_requested()) {
                if (frontier.empty()) {
                    // TODO: ask for more work
                }

                auto node = frontier.front();
                frontier.pop();
                for (const auto &child: problem.expand(node)) {
                    if (token.stop_requested()) break;
                    State child_state = child->state();
                    if (problem.is_goal(child_state)) return child;
                    frontier.push(child);
                }
            }
        }

    private:
        std::queue<std::shared_ptr<Node<State>>> frontier;
    };
}


namespace parallel_bfs {
    /// In order to avoid data races, ParallelBFSTasks only works with tree-like search.
    template<Searchable State, std::derived_from<BaseTransitionModel<State>> TM>
    [[nodiscard]] std::shared_ptr<Node<State>> multithread_bfs(const Problem<State, TM> &problem) {
        std::queue<std::shared_ptr<Node<State>>> frontier({std::make_shared<Node<State>>(problem.initial())});
        // TODO: use ThreadDirector to manage threads
        return nullptr;
    }
}
#endif //PARALLEL_BFS_MULTITHREAD_BFS_H
