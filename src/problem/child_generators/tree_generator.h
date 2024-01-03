//
// Created by Alejandro Fernández on 30/11/2023.
//

#ifndef PARALLEL_BFS_TREE_GENERATOR_H
#define PARALLEL_BFS_TREE_GENERATOR_H

#include <algorithm>
#include <random>
#include "../../state/tree_state.h"
#include "../child_generator.h"

class TreeGenerator : public ChildGenerator<TreeState> {
public:
    explicit TreeGenerator(unsigned int max_depth, state_t max_actions, double avg_actions,
                           std::default_random_engine rand_engine)
            : _max_depth{max_depth}, _possible_actions(max_actions), _prng_engine{rand_engine},
              _bino_dist{max_actions, avg_actions / max_actions} {
        std::iota(_possible_actions.begin(), _possible_actions.end(), 0);
    }

    /// Generates a vector of new valid states from "state".
    [[nodiscard]] std::vector<TreeState> operator()(const TreeState &state) override {
        // If we have already reached max_depth return an empty vector
        if (state.depth() >= _max_depth) return {};

        // Generate a new state for each possible action (generated randomly)
        std::vector<TreeState> new_states;
        std::ranges::transform(
                get_rand_actions(),
                std::back_inserter(new_states),
                [state](state_t a) { return TreeState{state, a}; }
        );
        return new_states;
    }

private:
    [[nodiscard]] std::vector<state_t> get_rand_actions() {
        state_t n{_bino_dist(_prng_engine)};
        std::ranges::shuffle(_possible_actions, _prng_engine);
        return {_possible_actions.cbegin(), _possible_actions.cbegin() + n};
    }

private:
    const unsigned int _max_depth;
    std::vector<state_t> _possible_actions;
    std::default_random_engine _prng_engine;
    std::binomial_distribution<state_t> _bino_dist;
};

#endif //PARALLEL_BFS_TREE_GENERATOR_H
