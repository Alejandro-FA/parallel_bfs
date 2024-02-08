//
// Created by Alejandro Fernández on 06/11/2023.
//

#ifndef PARALLEL_BFS_PARALLEL_BFS_TASKS_H
#define PARALLEL_BFS_PARALLEL_BFS_TASKS_H

#include <vector>
#include <memory>
#include <future>
#include <thread>
#include <iostream>
#include "bfs.h"


namespace parallel_bfs {
    /// In order to avoid data races, ParallelBFSTasks only works with tree-like search.
    template<Searchable State, std::derived_from<BaseTransitionModel<State>> TM>
    [[nodiscard]] std::shared_ptr<Node<State>> parallel_bfs_tasks(const Problem<State, TM> &problem) {
        std::queue<std::shared_ptr<Node<State>>> frontier({std::make_shared<Node<State>>(problem.initial())});

        std::vector<std::future<std::shared_ptr<Node<State>>>> futures;
        unsigned int cpu_cores = std::thread::hardware_concurrency();
        std::stop_source stop_source{};

        while(!frontier.empty() && futures.size() < cpu_cores) {
            auto node = frontier.front();
            frontier.pop();
            for (const auto &child: problem.expand(node)) {
                std::packaged_task task{ [child, &problem, &stop_source]() {
                    auto solution {detail::bfs(child, problem, stop_source.get_token())};
                    if (solution != nullptr) stop_source.request_stop();
                    return solution;
                }};
                futures.push_back(task.get_future());
                std::jthread thread{std::move(task)};
            }
        }

        for (auto &future: futures)
            if (auto solution = future.get(); solution != nullptr) return solution;

        return nullptr;
    }
}
#endif //PARALLEL_BFS_PARALLEL_BFS_TASKS_H
