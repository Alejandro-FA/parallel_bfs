//
// Created by Alejandro Fernández on 06/11/2023.
//

#ifndef PARALLEL_BFS_SYNC_BFS_H
#define PARALLEL_BFS_SYNC_BFS_H

#include <queue>
#include "parallel_bfs/search/problem.h"
#include "parallel_bfs/search/node.h"

namespace parallel_bfs {
    enum class SearchType {
        graph [[maybe_unused]], tree_like [[maybe_unused]],
    };

    template<SearchType type = SearchType::tree_like>
    class SyncBFS { // FIXME: Change this from Breadth First Search to Best First Search
    public:
        template<Searchable State, std::derived_from<BaseTransitionModel<State>> TM>
        [[nodiscard]] std::shared_ptr<Node<State>> operator()(const Problem<State, TM> &problem) const {
            auto init_node = std::make_shared<Node<State>>(problem.initial());
            if (problem.is_goal(problem.initial())) return init_node;

            std::queue<std::shared_ptr<Node<State>>> frontier({init_node});
            std::unordered_set<State> reached({init_node->state()}); // Keep track of reached states in graph-search type

            while (!frontier.empty()) {
                auto node = frontier.front();
                frontier.pop();
                for (const auto &child: problem.expand(node)) {
                    State child_state = child->state();
                    if (problem.is_goal(child_state)) return child;

                    if constexpr (type == SearchType::graph) { // Graph search
                        if (!reached.contains(child_state)) {
                            reached.insert(child_state);
                            frontier.push(child);
                        }
                    } else { // Tree-like search
                        frontier.push(child);
                    }
                }
            }
            return nullptr;
        }
    };
}

#endif //PARALLEL_BFS_SYNC_BFS_H
