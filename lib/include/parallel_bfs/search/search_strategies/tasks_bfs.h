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
        std::queue<std::shared_ptr<Node<State>>> frontier({std::make_shared<Node<State>>(problem.initial())});
        unsigned int min_starting_points = std::thread::hardware_concurrency();

        // First fill the frontier with enough starting points
        while(!frontier.empty() && frontier.size() < min_starting_points) {
            auto node = frontier.front();
            frontier.pop();
            // frontier.push_range(problem.expand(node)); // TODO: Use this when available
            for (const auto &child: problem.expand(node)) frontier.push(child);
        }

        // Then start a parallel search from each starting point
        std::vector<std::jthread> threads;
        std::vector<std::future<std::shared_ptr<Node<State>>>> futures;
        std::stop_source stop_source{};

        while (!frontier.empty()) {
            auto node = frontier.front();
            frontier.pop();
            std::packaged_task task{ [node, &problem, &stop_source]() {
                auto solution {detail::bfs(node, problem, stop_source.get_token())};
                if (solution != nullptr) stop_source.request_stop();
                return solution;
            }};

            futures.push_back(task.get_future());
            threads.emplace_back(std::move(task));
        }

        // Wait for the first solution to be found and return it
        for (auto &future: futures)
            if (auto solution = future.get(); solution != nullptr) return solution;

        return nullptr;
    }

    // /// In order to avoid data races, tasks_bfs only works with tree-like search.
    // template<Searchable State, std::derived_from<BaseTransitionModel<State>> TM>
    // [[nodiscard]] std::shared_ptr<Node<State>> tasks_bfs(const Problem<State, TM> &problem) {
    //     std::queue<std::shared_ptr<Node<State>>> frontier({std::make_shared<Node<State>>(problem.initial())});
    //
    //     std::vector<std::future<std::shared_ptr<Node<State>>>> futures;
    //     unsigned int min_spawned_tasks = std::thread::hardware_concurrency();
    //     std::stop_source stop_source{};
    //
    //     while(!frontier.empty() && futures.size() < min_spawned_tasks) {
    //         auto node = frontier.front();
    //         frontier.pop();
    //         for (const auto &child: problem.expand(node)) {
    //             std::packaged_task task{ [child, &problem, &stop_source]() {
    //                 auto solution {detail::bfs(child, problem, stop_source.get_token())};
    //                 if (solution != nullptr) stop_source.request_stop();
    //                 return solution;
    //             }};
    //             futures.push_back(task.get_future());
    //             std::jthread thread{std::move(task)};
    //         }
    //     }
    //
    //     for (auto &future: futures)
    //         if (auto solution = future.get(); solution != nullptr) return solution;
    //
    //     return nullptr;
    // }
}
#endif //PARALLEL_BFS_TASKS_BFS_H
