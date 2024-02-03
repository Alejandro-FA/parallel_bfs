//
// Created by alejandro on 18/11/23.
//

#ifndef PARALLEL_BFS_PROJECT_BASIC_GRAPH_GENERATOR_H
#define PARALLEL_BFS_PROJECT_BASIC_GRAPH_GENERATOR_H

#include "basic_graph.h"


namespace parallel_bfs {
    template<detail::UnsignedInteger T>
    class BasicGraphGenerator : public RandomFactory<T, BasicGraph<T>> {
    public:
        explicit BasicGraphGenerator(unsigned int num_states, unsigned num_actions)
                : _num_states{num_states}, _num_actions{num_actions} {
            if (num_states < 1) throw std::invalid_argument("A BasicGraph must have at least 1 state.");
            if (num_actions > num_states)
                throw std::invalid_argument(
                        "Each node of the graph cannot have more edges than the total number of nodes.");
        }

    protected:
        [[nodiscard]] T get_initial() override { return this->get_random_value(_udist); }

        [[nodiscard]] std::unordered_set<T> get_goal_states() override { return {this->get_random_value(_udist)}; }

        /// Builds an Adjacency List representation of a graph. Might not return a connected graph.
        [[nodiscard]] BasicGraph<T> get_transition_model() override {
            // Create a graph with _num_states nodes and no edges.
            BasicGraph<T> graph(_num_states);

            // For each state, create up to [_max_actions] children states (graph edges)
            for (unsigned int i = 0; i < _num_states; ++i) {
                for (unsigned int j = 0; j < _num_actions; ++j) {
                    T rand_node{this->get_random_value(_udist)};
                    graph[i].insert(rand_node);
                }
            }

            return graph;
        }

    private:
        const unsigned int _num_states;
        const unsigned int _num_actions;
        std::uniform_int_distribution<T> _udist{0, static_cast<T>(_num_states - 1)};
    };
}

#endif //PARALLEL_BFS_BASIC_GRAPH_GENERATOR_H
