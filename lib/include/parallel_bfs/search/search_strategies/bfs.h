//
// Created by alejandro on 02/02/24.
//

#ifndef PARALLEL_BFS_PROJECT_BFS_H
#define PARALLEL_BFS_PROJECT_BFS_H

#include <memory>
#include <queue>
#include <deque>
#include <stop_token>
#include "../problem.h"
#include "../node.h"
#include "../state.h"
#include "../transition_model.h"


namespace parallel_bfs::detail {
    /// NOTE: We use tree-like search, so we don't need to check for repeated states
    template<Searchable State, std::derived_from<BaseTransitionModel<State>> TM>
    [[nodiscard]] std::shared_ptr<Node<State>>
    _bfs(std::deque<std::shared_ptr<Node<State>>> &frontier,
         const Problem<State, TM> &problem,
         std::stop_source ssource = std::stop_source{},
         std::size_t limit = std::numeric_limits<std::size_t>::max()) {
        while (!frontier.empty() && !ssource.stop_requested() && frontier.size() < limit) {
            auto node = frontier.front();
            frontier.pop_front();
            if (problem.is_goal(node->state())) {
                ssource.request_stop();
                return node;
            }
            // frontier.push_range(problem.expand(node)); // TODO: Use this when available
            for (const auto &child: problem.expand(std::move(node))) frontier.push_back(child);
        }

        return nullptr;
    }


    /// NOTE: We use tree-like search, so we don't need to check for repeated states
    template<Searchable State, std::derived_from<BaseTransitionModel<State>> TM>
    [[nodiscard]] std::shared_ptr<Node<State>>
    interruptible_bfs(std::deque<std::shared_ptr<Node<State>>> &frontier, const Problem<State, TM> &problem, std::stop_source ssource = std::stop_source{}) {
        return _bfs(frontier, problem, ssource);
    }


    template<Searchable State, std::derived_from<BaseTransitionModel<State>> TM>
    [[nodiscard]] std::shared_ptr<Node<State>>
    interruptible_bfs(std::shared_ptr<Node<State>> init_node, const Problem<State, TM> &problem, std::stop_source ssource = std::stop_source{}) {
        std::deque<std::shared_ptr<Node<State>>> frontier({std::move(init_node)});
        return _bfs(frontier, problem, ssource);
    }


    /// Search until solution is found or frontier fills up to limit
    template<Searchable State, std::derived_from<BaseTransitionModel<State>> TM>
    [[nodiscard]] std::shared_ptr<Node<State>>
    bfs_with_limit(std::deque<std::shared_ptr<Node<State>>> &frontier, const Problem<State, TM> &problem, std::size_t limit) {
        return _bfs(frontier, problem, std::stop_source{}, limit);
    }
}

#endif //PARALLEL_BFS_PROJECT_BFS_H
