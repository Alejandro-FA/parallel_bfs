//
// Created by Alejandro Fernández on 10/02/2024.
//

#ifndef PARALLEL_BFS_PROJECT_FOREACH_START_BFS_H
#define PARALLEL_BFS_PROJECT_FOREACH_START_BFS_H

#include <memory>
#include <thread>
#include <execution>
#include <algorithm>
#include <deque>
#include "bfs.h"


namespace parallel_bfs {
    /// In order to avoid data races, foreach_bfs only works with tree-like search.
    template<Searchable State, std::derived_from<BaseTransitionModel<State>> TM>
    [[nodiscard]] std::shared_ptr<Node<State>> foreach_start_bfs(const Problem<State, TM> &problem) {
        std::deque<std::shared_ptr<Node<State>>> frontier({std::make_shared<Node<State>>(problem.initial())});
        unsigned int min_starting_points = std::thread::hardware_concurrency();

        // First fill the frontier with enough starting points
        auto possible_solution = detail::bfs_with_limit(frontier, problem, min_starting_points);
        if (possible_solution != nullptr) return possible_solution;

        // Then start a parallel search from each starting point
        std::stop_source stop_source{};
        std::atomic<std::shared_ptr<Node<State>>> solution{nullptr};

        std::for_each(std::execution::par, frontier.cbegin(), frontier.cend(), [&problem, stop_source, &solution](const auto &node) {
            auto possible_solution = detail::interruptible_bfs(node, problem, stop_source);
            if (possible_solution != nullptr) solution.store(possible_solution);
        });

        return solution.load();
    }
}

#endif //PARALLEL_BFS_PROJECT_FOREACH_START_BFS_H
