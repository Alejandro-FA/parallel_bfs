//
// Created by Alejandro Fernández on 06/11/2023.
//

#ifndef PARALLEL_BFS_PARALLEL_BFS_TASKS_H
#define PARALLEL_BFS_PARALLEL_BFS_TASKS_H

#include <queue>
#include <future>
#include <thread>
#include "bfs.h"


namespace parallel_bfs {
    /// In order to avoid data races, ParallelBFSTasks only works with tree-like search.
    template<Searchable State, std::derived_from<BaseTransitionModel<State>> TM>
    [[nodiscard]] std::shared_ptr<Node<State>> parallel_bfs_tasks(const Problem<State, TM> &problem) {
        auto init_node = std::make_shared<Node<State>>(problem.initial());
        // if (problem.is_goal(init_node->state())) return init_node;
        //
        // for (const auto &child: problem.expand(init_node)) {
        //
        // }
        auto lambda = [init_node, &problem]() { return detail::bfs(init_node, problem); };
        std::packaged_task task(lambda);
        std::future<std::shared_ptr<Node<State>>> future {task.get_future()};
        std::jthread thread{std::move(task)};
        return future.get();
    }
}
#endif //PARALLEL_BFS_PARALLEL_BFS_TASKS_H
