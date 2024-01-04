//
// Created by alejandro on 18/11/23.
//

#ifndef PARALLEL_BFS_BASIC_GRAPH_BUILDER_H
#define PARALLEL_BFS_BASIC_GRAPH_BUILDER_H

#include "random_builder.h"
#include "../../problem/transition_models/basic_graph.h"

class BasicGraphBuilder : public RandomBuilder<uint32_t> {
public:
    explicit BasicGraphBuilder(uint32_t num_states, uint32_t num_actions, std::optional<unsigned int> seed = std::nullopt)
            : RandomBuilder(seed), _num_states{num_states}, _num_actions{num_actions} {}

    [[nodiscard]] uint32_t get_initial() override { return _udist(_prng_engine); }

    [[nodiscard]] std::unordered_set<uint32_t> get_goal() override { return {_udist(_prng_engine)}; }

    /// Builds an Adjacency List representation of a graph. Might not return a connected graph.
    [[nodiscard]] std::unique_ptr<BaseTransitionModel<uint32_t>> get_transition_model() override {
        // Create empty graph and add each possible state as a key of the map.
        BasicGraph::graph_t graph;
        graph.reserve(_num_states);
        for (uint32_t i = 0; i < _num_states; ++i) {
            graph.insert({i, std::unordered_set<uint32_t>()});
        }

        // For each state, create up to [_max_actions] children states (graph edges)
        for (uint32_t i = 0; i < _num_states; ++i) {
            for (uint32_t j = 0; j < _num_actions; ++j) {
                uint32_t rand_node{_udist(_prng_engine)};
                graph[i].insert(rand_node);
            }
        }

        return std::make_unique<BasicGraph>(std::move(graph));
    }

private:
    const uint32_t _num_states;
    const uint32_t _num_actions;
    std::uniform_int_distribution<uint32_t> _udist{0, _num_states - 1};
};

#endif //PARALLEL_BFS_BASIC_GRAPH_BUILDER_H
