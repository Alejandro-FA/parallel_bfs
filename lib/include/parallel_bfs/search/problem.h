//
// Created by alejandro on 18/11/23.
//

#ifndef PARALLEL_BFS_PROBLEM_H
#define PARALLEL_BFS_PROBLEM_H

#include <vector>
#include <memory>
#include <unordered_set>
#include <thread>
#include <chrono>
#include "node.h"
#include "transition_model.h"

namespace parallel_bfs {
    template<Searchable State, std::derived_from<BaseTransitionModel<State>> TM>
    class Problem {
    public:
        explicit Problem(State initial, std::unordered_set<State> &&goal_states, TM &&tm)
            : _initial{std::move(initial)}, _goal_states{std::move(goal_states)}, _transition_model{std::move(tm)} {}

        [[nodiscard]] State initial() const { return _initial; }

        [[nodiscard]] bool is_goal(const State &state) const {
            auto start = std::chrono::high_resolution_clock::now();
            while (std::chrono::high_resolution_clock::now() - start < _workload_delay); // busy wait
            return _goal_states.contains(state);
        }

        [[nodiscard]] std::unordered_set<State> goal_states() const { return _goal_states; }

        // TODO: perhaps change this to a coroutine when std::generator (C++23) is implemented in gcc/clang
        [[nodiscard]] std::vector<std::shared_ptr<Node<State>>> expand(const std::shared_ptr<Node<State>> &node) const {
            std::vector<std::shared_ptr<Node<State>>> expanded_nodes;
            for (auto &[new_state, cost]: _transition_model.next_states(node->state())) {
                auto new_node = std::make_shared<Node<State>>(std::move(new_state), node, cost);
                expanded_nodes.push_back(new_node);
            }
            return expanded_nodes;
        }

        [[nodiscard]] const TM &transition_model() const { return _transition_model; }

        void set_workload_delay(std::chrono::microseconds us) {_workload_delay = us; }

    private:
        State _initial;
        std::unordered_set<State> _goal_states;
        TM _transition_model;
        std::chrono::microseconds _workload_delay{0};
    };


    template<Searchable State, std::derived_from<BaseTransitionModel<State>> TM>
    std::ostream &operator<<(std::ostream &os, const Problem<State, TM> &p) {
        os << "Initial state: " << p.initial() << "\nGoal state(s):";
        for (const State &goal: p.goal_states()) os << "\n - " << goal;
        return os;
    }
}

#endif //PARALLEL_BFS_PROBLEM_H
