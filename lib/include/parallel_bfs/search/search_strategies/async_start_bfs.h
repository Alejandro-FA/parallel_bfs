//
// Created by alejandro on 30/01/24.
//

#ifndef PARALLEL_BFS_PROJECT_ASYNC_START_BFS_H
#define PARALLEL_BFS_PROJECT_ASYNC_START_BFS_H

#include <future>
#include <iostream>
#include <vector>
#include <memory>
#include <thread>
#include <deque>
#include "bfs.h"
#include "../node.h"
#include "../state.h"
#include "../transition_model.h"


namespace parallel_bfs {
    /// In order to avoid data races, async_start_bfs only works with tree-like search.
    template<Searchable State, std::derived_from<BaseTransitionModel<State>> TM>
    [[nodiscard]] std::shared_ptr<Node<State>> async_start_bfs(const Problem<State, TM> &problem) {
        std::deque<std::shared_ptr<Node<State>>> frontier{std::make_shared<Node<State>>(problem.initial())};
        unsigned int min_starting_points = std::thread::hardware_concurrency() * 2;

        // First fill the frontier with enough starting points
        auto possible_solution = detail::bfs_with_limit(frontier, problem, min_starting_points);
        if (possible_solution != nullptr) return possible_solution;

        // Then start a parallel search from each starting point
        std::vector<std::future<std::shared_ptr<Node<State>>>> futures;
        std::stop_source stop_source{};

        while (!frontier.empty()) {
            auto future = std::async(std::launch::async, [&problem, stop_source](std::shared_ptr<Node<State>> node) {
                return detail::interruptible_bfs(std::move(node), problem, stop_source);
            }, frontier.front());
            frontier.pop_front();
            futures.push_back(std::move(future));
        }

        std::shared_ptr<Node<State>> solution{nullptr};
        for (auto &future: futures) // Ensure that all threads have finished to avoid data races
            if (auto result = future.get(); result != nullptr) solution = result;
        return solution;
    }
}

#endif //PARALLEL_BFS_PROJECT_ASYNC_START_BFS_H
