//
// Created by alejandro on 18/11/23.
//

#ifndef PARALLEL_BFS_PROBLEM_H
#define PARALLEL_BFS_PROBLEM_H

#include <vector>
#include <memory>
#include <unordered_set>
#include "transition_model.h"
#include "node.h"

namespace parallel_bfs {
    template<Searchable State>
    class Problem {
    public:
        explicit Problem(State initial, std::unordered_set<State> &&goal,
                         std::unique_ptr<BaseTransitionModel<State>> transition_model) :
                _initial{std::move(initial)}, _goal{std::move(goal)}, _transition_model{std::move(transition_model)} {}

        [[nodiscard]] State initial() const { return _initial; }

        [[nodiscard]] std::unordered_set<State> goal() const { return _goal; }

        [[nodiscard]] bool is_goal(const State &state) const { return _goal.contains(state); }

        // TODO: perhaps change this to a coroutine when std::generator (C++23) is implemented in gcc/clang
        [[nodiscard]] std::vector<std::shared_ptr<Node<State>>> expand(const std::shared_ptr<Node<State>> &node) const {
            std::vector<std::shared_ptr<Node<State>>> expanded_nodes;
            for (auto &[new_state, cost]: _transition_model->next_states(node->state())) {
                expanded_nodes.push_back(std::make_shared<Node<State>>(std::move(new_state), node, cost));
            }
            return expanded_nodes;
        }

    private:
        const State _initial;
        const std::unordered_set<State> _goal;
        const std::unique_ptr<BaseTransitionModel<State>> _transition_model;
    };


    template<Printable State>
    std::ostream &operator<<(std::ostream &os, const Problem<State> &p) {
        os << "Initial state: " << p.initial() << "\nGoal state(s):";
        for (const State &goal: p.goal()) os << "\n - " << goal;
        return os;
    }
}

#endif //PARALLEL_BFS_PROBLEM_H
