//
// Created by Alejandro Fernández on 04/11/2023.
//

#ifndef PARALLEL_BFS_PROBLEM_H
#define PARALLEL_BFS_PROBLEM_H

#include <unordered_map>
#include <unordered_set>
#include "problem_generator.h"
#include "state.h"
#include "node.h"

// NOTE: Perhaps the problem does not have a solution, or the goal state to is equal to the initial state,
// or one state has less "valid actions" than [num_actions]. All of this is intended to ensure correct
// behaviour with edge cases.
class Problem {
public:
    explicit Problem(state_space_size_t num_states, state_space_size_t num_actions, std::optional<unsigned int> seed = std::nullopt)
            : _pg{num_states, seed},
              _initial{_pg.get_rand_state()},
              _goal{_pg.get_rand_state()},
              _graph{_pg.get_rand_graph(num_actions)} {}

    [[nodiscard]] State initial() const { return _initial; }

    [[nodiscard]] State goal() const { return _goal; }

    [[nodiscard]] bool is_goal(State state) const { return state == goal(); }

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

    void print(bool include_graph) {
        std::cout << "\nInitial state: " << initial() << "\nGoal state: " << goal();
        if (include_graph) {
            std::cout << "\nAdjacency list:";
            for (const auto &[state, state_neighbours]: _graph) {
                std::cout << "\n   " << state << ": ";
                for (const auto &neighbour: state_neighbours) std::cout << neighbour << " ";
            }
        }
        std::cout << std::endl;
    }

private:
    ProblemGenerator _pg;
    const State _initial;
    const State _goal;
    const std::unordered_map<State, std::unordered_set<State>> _graph;
};


#endif //PARALLEL_BFS_PROBLEM_H
