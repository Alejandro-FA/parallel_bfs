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
    template<State State>
    class Problem {
    public:
        explicit Problem(State initial, std::unordered_set<State> &&goal_states,
                         std::unique_ptr<BaseTransitionModel<State>> transition_model) :
                _initial{std::move(initial)}, _goal_states{std::move(goal_states)},
                _transition_model{std::move(transition_model)} {}

        [[nodiscard]] State initial() const { return _initial; }


        [[nodiscard]] bool is_goal(const State &state) const { return _goal_states.contains(state); }

        // TODO: perhaps change this to a coroutine when std::generator (C++23) is implemented in gcc/clang
        [[nodiscard]] std::vector<std::shared_ptr<Node<State>>> expand(const std::shared_ptr<Node<State>> &node) const {
            std::vector<std::shared_ptr<Node<State>>> expanded_nodes;
            for (auto &[new_state, cost]: _transition_model->next_states(node->state())) {
                auto new_node = std::make_shared<Node<State>>(std::move(new_state), node, cost);
                expanded_nodes.push_back(new_node);
            }
            return expanded_nodes;
        }

        [[nodiscard]] std::unordered_set<State> goal_states() const { return _goal_states; }

        [[nodiscard]] const BaseTransitionModel<State> &transition_model() const { return *_transition_model; }

    private:
        const State _initial;
        const std::unordered_set<State> _goal_states;
        const std::unique_ptr<BaseTransitionModel<State>> _transition_model;
    };

    template<State State>
    std::ostream &operator<<(std::ostream &os, const Problem<State> &p) {
        os << "Initial state: " << p.initial() << "\nGoal state(s):";
        for (const State &goal: p.goal_states()) os << "\n - " << goal;
        return os;
    }
}

#endif //PARALLEL_BFS_PROBLEM_H
