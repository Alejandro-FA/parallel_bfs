//
// Created by Alejandro Fernández on 06/11/2023.
//

#ifndef PARALLEL_BFS_SYNC_BFS_H
#define PARALLEL_BFS_SYNC_BFS_H

#include <queue>

enum class SearchType { graph [[maybe_unused]], tree_like [[maybe_unused]], };

template<SearchType type>
class SyncBFS {
public:
    template<typename State>
    [[nodiscard]] std::shared_ptr<Node<State>> operator()(const Problem<State> &problem) const {
        auto init_node = std::make_shared<Node<State>>(problem.initial());
        std::queue<std::shared_ptr<Node<State>>> frontier({init_node});
        std::unordered_set<State> reached({init_node->state()}); // Keep track of reached states in graph-search type

        while (!frontier.empty()) {
            auto node = frontier.front();
            frontier.pop();
            auto children = problem.expand(node);
            for (const auto &child: children) {
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

#endif //PARALLEL_BFS_SYNC_BFS_H
