//
// Created by alejandro on 02/02/24.
//

#ifndef PARALLEL_BFS_PROJECT_BFS_H
#define PARALLEL_BFS_PROJECT_BFS_H

#include <memory>
#include <queue>
#include <stop_token>


namespace parallel_bfs {
    enum class SearchType {
        graph [[maybe_unused]], tree_like [[maybe_unused]],
    };
}


namespace parallel_bfs::detail {
    /// TODO: Change this from Breadth First Search to Best First Search
    template<Searchable State, std::derived_from<BaseTransitionModel<State>> TM, SearchType type = SearchType::tree_like>
    [[nodiscard]] std::shared_ptr<Node<State>> bfs(std::shared_ptr<Node<State>> init_node, const Problem<State, TM> &problem, std::stop_token st = std::stop_token{}) {
        if (problem.is_goal(init_node->state())) return init_node;
        std::queue<std::shared_ptr<Node<State>>> frontier({init_node});
        std::unordered_set<State> reached;
        if constexpr (type == SearchType::graph) reached.insert(init_node->state());

        while (!frontier.empty() && !st.stop_requested()) {
            auto node = frontier.front();
            frontier.pop();
            for (const auto &child: problem.expand(node)) {
                if (st.stop_requested()) break;
                State child_state = child->state();
                if (problem.is_goal(child_state)) return child;
                if constexpr (type == SearchType::graph) {
                    if (reached.find(child_state) == reached.end()) {
                        reached.insert(child_state);
                        frontier.push(child);
                    }
                } else if constexpr (type == SearchType::tree_like) {
                    frontier.push(child);
                }
            }
        }

        return nullptr;
    }
}

#endif //PARALLEL_BFS_PROJECT_BFS_H
