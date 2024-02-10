//
// Created by Alejandro Fernández on 10/02/2024.
//

#ifndef PARALLEL_BFS_PROJECT_FOREACH_START_BFS_H
#define PARALLEL_BFS_PROJECT_FOREACH_START_BFS_H

#include <iostream>
#include <memory>
#include <thread>
#include <execution>
#include <deque>
#include "bfs.h"


namespace parallel_bfs {
    /// In order to avoid data races, foreach_bfs only works with tree-like search.
    template<Searchable State, std::derived_from<BaseTransitionModel<State>> TM>
    [[nodiscard]] std::shared_ptr<Node<State>> foreach_start_bfs(const Problem<State, TM> &problem) {
        std::deque<std::shared_ptr<Node<State>>> frontier({std::make_shared<Node<State>>(problem.initial())});
        unsigned int min_starting_points = std::thread::hardware_concurrency();

        // First fill the frontier with enough starting points
        while (!frontier.empty() && frontier.size() < min_starting_points) {
            auto node = frontier.front();
            frontier.pop_front();
            // frontier.push_range(problem.expand(node)); // TODO: Use this when available
            for (const auto &child: problem.expand(node)) frontier.push_back(child);
        }

        // Then start a parallel search from each starting point
        std::stop_source stop_source{};
        std::atomic<std::shared_ptr<Node<State>>> solution{nullptr};

        std::for_each(std::execution::par, frontier.begin(), frontier.end(), [&problem, &stop_source, &solution](auto node) {
            auto partial_solution{detail::bfs(node, problem, stop_source.get_token())};
            if (partial_solution != nullptr) {
                stop_source.request_stop();
                solution.store(partial_solution);
            }
        });

        return solution.load();
    }
}

#endif //PARALLEL_BFS_PROJECT_FOREACH_START_BFS_H
