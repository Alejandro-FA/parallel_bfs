//
// Created by Alejandro Fernández on 05/11/2023.
//

#ifndef PARALLEL_BFS_PROBLEM_GENERATOR_H
#define PARALLEL_BFS_PROBLEM_GENERATOR_H

#include <random>
#include <unordered_map>
#include <unordered_set>
#include "state.h"

class ProblemGenerator {
public:
    /// Constructs a random problem. For each state there will be up to [num_actions] possible actions.
    explicit ProblemGenerator(state_space_size_t num_states, std::optional<unsigned int> seed = std::nullopt)
            : _num_states{num_states},
              _prng_engine{get_prng_engine(seed)},
              _dist(0, num_states - 1) {}

    [[nodiscard]] State get_rand_state() { return State{_dist(_prng_engine)}; }

    /// Builds an Adjacency List representation of a graph. Might not return a connected graph.
    [[nodiscard]] auto get_rand_graph(state_space_size_t children_per_state) {
        // Create empty graph and add each possible state as a key of the map.
        std::unordered_map<State, std::unordered_set<State>> g;
        g.reserve(_num_states);
        for (long i = _num_states - 1; i >= 0; --i) g.emplace(i, std::unordered_set<State>());

        // For each state, create up to [_num_actions] children states (graph edges)
        for (state_space_size_t i = 0; i < _num_states; ++i) {
            State state_i = State{i};
            for (state_space_size_t j = 0; j < children_per_state; ++j) {
                State state_j = State{_dist(_prng_engine)};
                g[state_i].insert(state_j);
            }
        }
        return g;
    }

private:
    state_space_size_t _num_states;
    std::default_random_engine _prng_engine;
    std::uniform_int_distribution<state_space_size_t> _dist;

    static std::default_random_engine get_prng_engine(std::optional<unsigned int> seed) {
        if (seed.has_value()) return std::default_random_engine{seed.value()};
        std::random_device r;
        std::seed_seq seed2{r(), r(), r(), r(), r(), r(), r(), r()};
        return std::default_random_engine{seed2};
    }
};

#endif //PARALLEL_BFS_PROBLEM_GENERATOR_H
