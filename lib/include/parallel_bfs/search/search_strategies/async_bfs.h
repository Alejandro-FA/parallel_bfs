//
// Created by alejandro on 09/02/24.
//

#ifndef PARALLEL_BFS_PROJECT_ASYNC_BFS_H
#define PARALLEL_BFS_PROJECT_ASYNC_BFS_H

#include <future>
#include <iostream>
#include <vector>
#include <memory>
#include <thread>
#include "../node.h"
#include "../state.h"
#include "../transition_model.h"


namespace parallel_bfs::detail {
    template<Searchable State, std::derived_from<BaseTransitionModel<State>> TM>
    [[nodiscard]] std::shared_ptr<Node<State>> async_bfs_recursive(std::shared_ptr<Node<State>> init_node, const Problem<State, TM> &problem, std::stop_source stop_source) {
        if (problem.is_goal(init_node->state())) return init_node;
        std::vector<std::future<std::shared_ptr<Node<State>>>> futures;

        if (stop_source.stop_requested()) return nullptr;

        for (const auto &child: problem.expand(init_node)) {
            if (stop_source.stop_requested()) break;
            auto future = std::async([&problem, stop_source](std::shared_ptr<Node<State>> node) {
                auto solution {detail::async_bfs_recursive(std::move(node), problem, stop_source)};
                if (solution != nullptr) stop_source.request_stop();
                return solution;
            }, std::move(child));
            futures.push_back(std::move(future));
        }

        std::shared_ptr<Node<State>> solution{nullptr};
        for (auto &future: futures) // Ensure that all threads have finished to avoid data races
            if (auto result = future.get(); result != nullptr) solution = result;
        return solution;
    }
}


namespace parallel_bfs {
    /// In order to avoid data races, ParallelBFSTasks only works with tree-like search.
    template<Searchable State, std::derived_from<BaseTransitionModel<State>> TM>
    [[nodiscard]] std::shared_ptr<Node<State>> async_bfs(const Problem<State, TM> &problem) {
        auto init_node = std::make_shared<Node<State>>(problem.initial());
        return detail::async_bfs_recursive(std::move(init_node), problem, std::stop_source{});
    }
}

#endif //PARALLEL_BFS_PROJECT_ASYNC_BFS_H
