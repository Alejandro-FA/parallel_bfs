//
// Created by alejandro on 18/11/23.
//

#ifndef PARALLEL_BFS_PROBLEM_H
#define PARALLEL_BFS_PROBLEM_H

#include <vector>

class Problem {
public:
    virtual ~Problem() = default;

    [[nodiscard]] std::shared_ptr<State> initial() const { return _initial; }

    [[nodiscard]] std::shared_ptr<State> goal() const { return _goal; }

    [[nodiscard]] bool is_goal(const State &state) const { return state == *goal(); }

    // TODO: Change this to a coroutine when std::generator (C++23) is implemented in g++
    [[nodiscard]] std::vector<std::shared_ptr<Node>> expand(const std::shared_ptr<Node> &node) const {
        const auto next_states = next_states_from(node->state());
        std::vector<std::shared_ptr<Node>> expanded_nodes;
        expanded_nodes.reserve(next_states.size());

        const auto new_cost = node->path_cost() + 1;
        for (const auto &ns: next_states) {
            expanded_nodes.push_back(make_shared<Node>(ns, node, new_cost));
        }

        return expanded_nodes;
    }

    [[nodiscard]] virtual std::vector<std::shared_ptr<State>> next_states_from(const std::shared_ptr<State> &state) const = 0;

protected:
    explicit Problem(std::shared_ptr<State> initial, std::shared_ptr<State> goal)
            : _initial{std::move(initial)}, _goal{std::move(goal)} {}

    const std::shared_ptr<State> _initial;
    const std::shared_ptr<State> _goal;
};

std::ostream &operator<<(std::ostream &os, const Problem &p) {
    return os << "\nInitial state: " << *p.initial() << "\nGoal state: " << *p.goal() << "\n";
}

#endif //PARALLEL_BFS_PROBLEM_H
