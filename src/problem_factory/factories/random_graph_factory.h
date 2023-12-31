//
// Created by alejandro on 18/11/23.
//

#ifndef PARALLEL_BFS_RANDOM_GRAPH_FACTORY_H
#define PARALLEL_BFS_RANDOM_GRAPH_FACTORY_H

#include "../random_problem_factory.h"
#include "../../problem/child_generators/graph_reader.h"

using state_t = uint32_t;

class RandomGraphFactory : public RandomProblemFactory<state_t> {
public:
    explicit RandomGraphFactory(state_t num_states, state_t num_actions, std::optional<unsigned int> seed = std::nullopt)
            : RandomProblemFactory(seed), _num_states{num_states}, _num_actions{num_actions},
              _udist(0, num_states - 1) {}

    [[nodiscard]] state_t make_initial() override { return _udist(_prng_engine); }

    [[nodiscard]] std::unordered_set<state_t> make_goal() override { return {_udist(_prng_engine)}; }

    /// Builds an Adjacency List representation of a graph. Might not return a connected graph.
    [[nodiscard]] std::unique_ptr<ChildGenerator<state_t>> make_child_generator() override {
        // Create empty graph and add each possible state as a key of the map.
        GraphReader<state_t>::graph_t g;
        g.reserve(_num_states);
        for (state_t i = 0; i < _num_states; ++i) {
            g.insert({i, std::unordered_set<state_t>()});
        }

        // For each state, create up to [_max_actions] children states (graph edges)
        for (state_t i = 0; i < _num_states; ++i) {
            for (state_t j = 0; j < _num_actions; ++j) {
                state_t rand_node{_udist(_prng_engine)};
                g[i].insert(rand_node);
            }
        }

        return std::make_unique<GraphReader<state_t>>(std::move(g));
    }

private:
    const state_t _num_states;
    const state_t _num_actions;
    std::uniform_int_distribution<state_t> _udist;
};

#endif //PARALLEL_BFS_RANDOM_GRAPH_FACTORY_H
