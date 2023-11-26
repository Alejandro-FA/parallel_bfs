//
// Created by Alejandro Fernández on 06/11/2023.
//

#ifndef PARALLEL_BFS_SYNC_BFS_H
#define PARALLEL_BFS_SYNC_BFS_H

#include <queue>

class SyncBFS final : public BFS {
public:
    [[nodiscard]] std::shared_ptr<Node> operator()(const Problem &problem) const override {
        std::shared_ptr<Node> init_node = std::make_shared<Node>(problem.initial());
        std::queue<std::shared_ptr<Node>> frontier({init_node});
        unordered_set_ptr<State> reached({init_node->state()}); // We use a set instead of a map because all actions have the same cost.

        while (!frontier.empty()) {
            auto node = frontier.front();
            frontier.pop();
            auto children = problem.expand(node);
            for (const auto &child : children) {
                std::shared_ptr<State> s = child->state();
                if (problem.is_goal(*s)) return child;
                if (!reached.contains(s)) {
                    reached.insert(s);
                    frontier.push(child);
                }
            }
        }
        return nullptr;
    }

    [[nodiscard]] std::string get_name() const override { return _name; }

private:
    inline static const std::string _name{"Synchronous BFS"};
};

#endif //PARALLEL_BFS_SYNC_BFS_H
