//
// Created by alejandro on 30/01/24.
//

#ifndef PARALLEL_BFS_PROJECT_PARALLEL_BFS_ASYNC_H
#define PARALLEL_BFS_PROJECT_PARALLEL_BFS_ASYNC_H

#include <future>
#include "../problem.h"
#include "bfs.h"


namespace parallel_bfs {
    /// In order to avoid data races, ParallelBFSTasks only works with tree-like search.
    template<Searchable State, std::derived_from<BaseTransitionModel<State>> TM>
    [[nodiscard]] std::shared_ptr<Node<State>> parallel_bfs_async(const Problem<State, TM> &problem) {
        auto init_node = std::make_shared<Node<State>>(problem.initial());
        // if (problem.is_goal(init_node->state())) return init_node;
        //
        // for (const auto &child: problem.expand(init_node)) {
        //
        // }
        auto lambda = [init_node, &problem]() { return detail::bfs(init_node, problem); };
        auto future = std::async(lambda);
        return future.get();
    }
}

#endif //PARALLEL_BFS_PROJECT_PARALLEL_BFS_ASYNC_H
