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


namespace parallel_bfs::detail {
    template<Searchable State, std::derived_from<BaseTransitionModel<State>> TM>
    [[nodiscard]] std::shared_ptr<Node<State>> async_bfs_task(std::shared_ptr<Node<State>> init_node, const Problem<State, TM> &problem, std::stop_source &stop_source) {
        if (problem.is_goal(init_node->state())) return init_node;
        std::vector<std::future<std::shared_ptr<Node<State>>>> futures;
        auto token = stop_source.get_token();

        if (token.stop_requested()) return nullptr;

        for (const auto &child: problem.expand(init_node)) {
            if (token.stop_requested()) break;
            auto future = std::async([child, &problem, &stop_source]() {
                auto solution {detail::async_bfs_task(child, problem, stop_source)};
                if (solution != nullptr) stop_source.request_stop();
                return solution;
            });
            futures.push_back(std::move(future));
        }

        for (auto &future: futures)
            if (auto solution = future.get(); solution != nullptr) return solution;

        return nullptr;
    }
}


namespace parallel_bfs {
    /// In order to avoid data races, ParallelBFSTasks only works with tree-like search.
    template<Searchable State, std::derived_from<BaseTransitionModel<State>> TM>
    [[nodiscard]] std::shared_ptr<Node<State>> async_bfs(const Problem<State, TM> &problem) {
        auto init_node = std::make_shared<Node<State>>(problem.initial());
        std::stop_source stop_source{};
        return detail::async_bfs_task(std::move(init_node), problem, stop_source);
    }
}

#endif //PARALLEL_BFS_PROJECT_ASYNC_BFS_H
