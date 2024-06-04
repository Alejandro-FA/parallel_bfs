//
// Created by Alejandro Fernández on 06/11/2023.
//

#ifndef PARALLEL_BFS_TASKS_BFS_H
#define PARALLEL_BFS_TASKS_BFS_H

#include <vector>
#include <memory>
#include <future>
#include <thread>
#include <queue>
#include "bfs.h"
#include "../problem.h"
#include "../node.h"
#include "../state.h"
#include "../transition_model.h"


namespace parallel_bfs {
    /// In order to avoid data races, ParallelBFSTasks only works with tree-like search.
    template<Searchable State, std::derived_from<BaseTransitionModel<State>> TM>
    [[nodiscard]] std::shared_ptr<Node<State>> tasks_bfs(const Problem<State, TM> &problem) {
        std::deque<std::shared_ptr<Node<State>>> frontier{std::make_shared<Node<State>>(problem.initial())};
        unsigned int min_starting_points = std::thread::hardware_concurrency() * 8;

        // First fill the frontier with enough starting points
        auto possible_solution = detail::bfs_with_limit(frontier, problem, min_starting_points);
        if (possible_solution != nullptr) return possible_solution;

        // Then start a parallel search from each starting point
        std::vector<std::jthread> threads(std::thread::hardware_concurrency()); // FIXME: Pass the number of threads as a parameter
        std::vector<std::future<std::shared_ptr<Node<State>>>> futures;
        std::stop_source stop_source{};

        // Split the frontier into as many subfrontiers as threads
        std::vector<std::deque<std::shared_ptr<Node<State>>> > subfrontiers(threads.size());
        while (!frontier.empty()) {
            for (std::size_t i = 0; i < subfrontiers.size() && !frontier.empty(); ++i) {
                subfrontiers[i].push_back(frontier.front());
                frontier.pop_front();
            }
        }

        // Launch a task for each subfrontier
        for (std::size_t i = 0; i < threads.size(); ++i) {
            std::packaged_task task{[&problem, stop_source](std::deque<std::shared_ptr<Node<State>>> &subfrontier) {
                return detail::interruptible_bfs(subfrontier, problem, stop_source);
            }};
            futures.push_back(task.get_future());
            threads[i] = std::jthread(std::move(task), std::ref(subfrontiers[i]));
        }

        std::shared_ptr<Node<State>> solution{nullptr};
        for (auto &future: futures) // Ensure that all threads have finished to avoid data races
            if (auto result = future.get(); result != nullptr) solution = result;
        return solution;
    }
}
#endif //PARALLEL_BFS_TASKS_BFS_H
