//
// Created by alejandro on 18/11/23.
//

#ifndef PARALLEL_BFS_RANDOM_GRAPH_FACTORY_H
#define PARALLEL_BFS_RANDOM_GRAPH_FACTORY_H

#include <unordered_map>
#include <unordered_set>
#include "problem_factory.h"
#include "../problem/random_graph.h"

class RandomGraphFactory : public RandomProblemFactory {
public:
    RandomGraphFactory(state_space_size_t num_states, state_space_size_t num_actions, std::optional<unsigned int> seed = std::nullopt)
            : RandomProblemFactory(seed), _num_states{num_states}, _num_actions{num_actions}, _dist(0, num_states - 1) {}

    /// Builds an Adjacency List representation of a graph. Might not return a connected graph.
    [[nodiscard]] std::unique_ptr<Problem> make_problem() override {
        State initial{_dist(_prng_engine)};
        State goal{_dist(_prng_engine)};

        // Create empty graph and add each possible state as a key of the map.
        std::unordered_map<State, std::unordered_set<State>> g;
        g.reserve(_num_states);
        for (long i = _num_states - 1; i >= 0; --i) g.emplace(i, std::unordered_set<State>());

        // For each state, create up to [_num_actions] children states (graph edges)
        for (state_space_size_t i = 0; i < _num_states; ++i) {
            State state_i = State{i};
            for (state_space_size_t j = 0; j < _num_actions; ++j) {
                State state_j = State{_dist(_prng_engine)};
                g[state_i].insert(state_j);
            }
        }

        return std::unique_ptr<RandomGraph>{new RandomGraph{initial, goal, std::move(g)}};
    }

private:
    state_space_size_t _num_states;
    state_space_size_t _num_actions;
    std::uniform_int_distribution<state_space_size_t> _dist;

};

#endif //PARALLEL_BFS_RANDOM_GRAPH_FACTORY_H
