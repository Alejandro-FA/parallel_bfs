//
// Created by Alejandro Fernández on 04/11/2023.
//

#ifndef PARALLEL_BFS_PROBLEM_H
#define PARALLEL_BFS_PROBLEM_H

#include <memory>
#include <unordered_map>
#include <vector>
#include <random>
#include <cmath>
#include <ostream>
#include "state.h"
#include "node.h"

// NOTE: Perhaps the problem does not have a solution. It is intended.
// It is also possible for the goal state to be equal to the initial state.
class Problem {
public:
    explicit Problem(state_space_size_t num_states) : Problem(num_states, get_rand_engine()) {}

    explicit Problem(state_space_size_t num_states, unsigned int seed)
            : Problem(num_states, std::default_random_engine{seed}) {}

    [[nodiscard]] State initial() const { return State{_initial}; }

    [[nodiscard]] State goal() const { return State{_goal}; }

    [[nodiscard]] bool is_goal(State state) const { return state == _goal; }

    // TODO: Change this to a coroutine when std::generator (C++23) is implemented in g++
    [[nodiscard]] std::vector<std::shared_ptr<Node>> expand(const std::shared_ptr<Node> &node) const {
        const auto next_states = _graph.at(node->state());
        std::vector<std::shared_ptr<Node>> expanded_nodes;
        expanded_nodes.reserve(next_states.size());

        const auto new_cost = node->path_cost() + 1;
        for (const auto &ns: next_states) {
            expanded_nodes.push_back(make_shared<Node>(ns, node, new_cost));
        }

        return expanded_nodes;
    }

    friend std::ostream &operator<<(std::ostream &, Problem const &);

private:
    const State _initial;
    const State _goal; // Could be equal to _initial state
    const std::unordered_map<State, std::vector<State>> _graph;

    explicit Problem(state_space_size_t num_states, std::default_random_engine rand_engine)
            : _initial{get_rand_state(num_states, rand_engine)},
              _goal{get_rand_state(num_states, rand_engine)},
              _graph{get_rand_graph(num_states, rand_engine)} {}

    static std::default_random_engine get_rand_engine() {
        std::random_device rd; // Seed with a real random value, if available
        std::seed_seq seed{rd(), rd(), rd(), rd(), rd()};
        return std::default_random_engine{seed};
    }

    static State get_rand_state(state_space_size_t num_states, std::default_random_engine &rand_engine) {
        std::uniform_int_distribution<state_space_size_t> uniform_dist(0, num_states - 1);
        return State{uniform_dist(rand_engine)};
    }

    // NOTE: Might not return a connected graph
    static std::unordered_map<State, std::vector<State>>
    get_rand_graph(state_space_size_t num_states, std::default_random_engine &rand_engine) {
        // Create empty graph as an Adjacency List
        std::unordered_map<State, std::vector<State>> g;
        g.reserve(num_states);
        for (int i = num_states - 1; i >= 0; --i) g.emplace(i, std::vector<State>());

        // Use Erdös-Rényi model to generate a random graph.
        double p = log(num_states) / (double) (num_states - 1); // Most of the time will return connected graphs
        std::bernoulli_distribution bernoulli(p);
        for (state_space_size_t i = 0; i < num_states; ++i) {
            for (state_space_size_t j = i + 1; j < num_states; ++j) {
                bool create_edge = bernoulli(rand_engine);
                if (create_edge) {
                    State state_i = State{i};
                    State state_j = State{j};
                    g[state_i].push_back(state_j);
                    g[state_j].push_back(state_i);
                }
            }
        }
        return g;
    }
};

std::ostream &operator<<(std::ostream &os, const Problem &p) {
    os << "Initial state: " << p.initial() << "\nGoal state: " << p.goal() << "\nAdjacency list:";
    for (const auto &[state, state_neighbours]: p._graph) {
        os << "\n   " << state << ": ";
        for (const auto &neighbour: state_neighbours) os << neighbour << " ";
    }
    return os << "\n";
}

#endif //PARALLEL_BFS_PROBLEM_H
