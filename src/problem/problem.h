//
// Created by alejandro on 18/11/23.
//

#ifndef PARALLEL_BFS_PROBLEM_H
#define PARALLEL_BFS_PROBLEM_H

#include <vector>
#include "../node.h"

template<Searchable T>
class Problem {
public:
    virtual ~Problem() = default;

    [[nodiscard]] T initial() const { return _initial; }

    [[nodiscard]] T goal() const { return _goal; }

    [[nodiscard]] bool is_goal(const T &state) const { return state == goal(); }

    // TODO: Change this to a coroutine when std::generator (C++23) is implemented in g++
    [[nodiscard]] std::vector<std::shared_ptr<Node<T>>> expand(const std::shared_ptr<Node<T>> &node) const {
        auto next_states = next_states_from(node->state());
        std::vector<std::shared_ptr<Node<T>>> expanded_nodes;
        expanded_nodes.reserve(next_states.size());

        int new_cost = node->path_cost() + 1;
        for (auto &ns: next_states) {
            expanded_nodes.push_back(std::make_shared<Node<T>>(std::move(ns), node, new_cost));
        }

        return expanded_nodes;
    }

    [[nodiscard]] virtual std::vector<T> next_states_from(const T &state) const = 0;

protected:
    explicit Problem(T initial, T goal) : _initial{std::move(initial)}, _goal{std::move(goal)} {}

    const T _initial;
    const T _goal;
};

template<Printable T>
std::ostream &operator<<(std::ostream &os, const Problem<T> &p) {
    return os << "Initial state: " << p.initial() << "\nGoal state: " << p.goal();
}

#endif //PARALLEL_BFS_PROBLEM_H
