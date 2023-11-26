//
// Created by alejandro on 18/11/23.
//

#ifndef PARALLEL_BFS_RANDOM_GRAPH_FACTORY_H
#define PARALLEL_BFS_RANDOM_GRAPH_FACTORY_H

#include "random_problem_factory.h"
#include "../problem/graph_problem.h"
#include "../state/graph_state.h"

class RandomGraphFactory : public RandomProblemFactory {
public:
    RandomGraphFactory(value_t num_states, value_t num_actions, std::optional<unsigned int> seed = std::nullopt)
            : RandomProblemFactory(seed), _num_states{num_states}, _num_actions{num_actions}, _dist(0, num_states - 1) {}

    /// Builds an Adjacency List representation of a graph. Might not return a connected graph.
    [[nodiscard]] std::unique_ptr<Problem> make_problem() override {
        std::shared_ptr<GraphState> initial = std::make_shared<GraphState>(_dist(_prng_engine));
        std::shared_ptr<GraphState> goal = std::make_shared<GraphState>(_dist(_prng_engine));

        // Create empty graph and add each possible state as a key of the map.
        std::vector<std::shared_ptr<State>> all_states;
        graph_t g;
        all_states.reserve(_num_states);
        g.reserve(_num_states);
        for (value_t i = 0; i < _num_states; ++i) {
            auto s{std::make_shared<GraphState>(i)};
            all_states.push_back(s);
            g.insert({s, unordered_set_ptr<State>()});
        }

        // For each state, create up to [_num_actions] children states (graph edges)
        for (value_t i = 0; i < _num_states; ++i) {
            for (value_t j = 0; j < _num_actions; ++j) {
                value_t rand_node{_dist(_prng_engine)};
                g[all_states[i]].insert(all_states[rand_node]);
            }
        }

        return std::make_unique<GraphProblem>(initial, goal, std::move(g));
    }

private:
    value_t _num_states;
    value_t _num_actions;
    std::uniform_int_distribution<value_t> _dist;
};

#endif //PARALLEL_BFS_RANDOM_GRAPH_FACTORY_H
