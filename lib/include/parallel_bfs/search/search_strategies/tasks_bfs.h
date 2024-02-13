//
// Created by Alejandro Fernández on 06/11/2023.
//

#ifndef PARALLEL_BFS_TASKS_BFS_H
#define PARALLEL_BFS_TASKS_BFS_H

#include <vector>
#include <memory>
#include <future>
#include <thread>
#include <iostream>
#include <queue>
#include "bfs.h"


namespace parallel_bfs {
    /// In order to avoid data races, ParallelBFSTasks only works with tree-like search.
    template<Searchable State, std::derived_from<BaseTransitionModel<State>> TM>
    [[nodiscard]] std::shared_ptr<Node<State>> tasks_bfs(const Problem<State, TM> &problem) {
        std::deque<std::shared_ptr<Node<State>>> frontier{std::make_shared<Node<State>>(problem.initial())};
        unsigned int min_starting_points = std::thread::hardware_concurrency();

        // First fill the frontier with enough starting points
        auto possible_solution = detail::bfs_with_limit(frontier, problem, min_starting_points);
        if (possible_solution != nullptr) return possible_solution;

        // Then start a parallel search from each starting point
        std::vector<std::jthread> threads;
        std::vector<std::future<std::shared_ptr<Node<State>>>> futures;
        std::stop_source stop_source{};

        while (!frontier.empty()) {
            std::packaged_task task{[&problem, stop_source](std::shared_ptr<Node<State>> init_node) {
                return detail::cooperative_bfs(std::move(init_node), problem, stop_source);
            }};
            futures.push_back(task.get_future());
            threads.emplace_back(std::move(task), frontier.front());
            frontier.pop_front();
        }

        // Wait for the first solution to be found and return it
        for (auto &future: futures)
            if (auto solution = future.get(); solution != nullptr) return solution;

        return nullptr;
    }
}
#endif //PARALLEL_BFS_TASKS_BFS_H
