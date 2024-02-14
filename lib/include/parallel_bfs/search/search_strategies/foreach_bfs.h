//
// Created by Alejandro Fernández on 10/02/2024.
//

#ifndef PARALLEL_BFS_PROJECT_FOREACH_BFS_H
#define PARALLEL_BFS_PROJECT_FOREACH_BFS_H

#include <iostream>
#include <memory>
#include <thread>
#include <execution>


namespace parallel_bfs::detail {
    template<Searchable State, std::derived_from<BaseTransitionModel<State>> TM>
    [[nodiscard]] std::shared_ptr<Node<State>> foreach_bfs_recursive(std::shared_ptr<Node<State>> init_node, const Problem<State, TM> &problem, std::stop_source stop_source) {
        if (problem.is_goal(init_node->state())) return init_node;

        if (stop_source.stop_requested()) return nullptr;

        auto children = problem.expand(init_node);
        std::atomic<std::shared_ptr<Node<State>>> solution{nullptr};

        std::for_each(std::execution::par, children.cbegin(), children.cend(), [&problem, stop_source, &solution](const auto &node) {
            auto possible_solution = detail::foreach_bfs_recursive(node, problem, stop_source);
            if (possible_solution != nullptr) {
                stop_source.request_stop();
                solution.store(possible_solution);
            }
        });

        return solution.load();
    }
}


namespace parallel_bfs {
    /// In order to avoid data races, ParallelBFSTasks only works with tree-like search.
    template<Searchable State, std::derived_from<BaseTransitionModel<State>> TM>
    [[nodiscard]] std::shared_ptr<Node<State>> foreach_bfs(const Problem<State, TM> &problem) {
        auto init_node = std::make_shared<Node<State>>(problem.initial());
        return detail::foreach_bfs_recursive(std::move(init_node), problem, std::stop_source{});
    }
}

#endif //PARALLEL_BFS_PROJECT_FOREACH_BFS_H
