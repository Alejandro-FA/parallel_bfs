//
// Created by Alejandro Fernández on 04/11/2023.
//

#ifndef PARALLEL_BFS_RANDOM_GRAPH_H
#define PARALLEL_BFS_RANDOM_GRAPH_H

#include <unordered_map>
#include <unordered_set>
#include <optional>
#include "problem.h"
#include "../state.h"
#include "../node.h"

class RandomGraphFactory;

// NOTE: Perhaps the problem does not have a solution, or the goal state to is equal to the initial state,
// or one state has less "valid actions" than [num_actions]. All of this is intended to ensure correct
// behaviour with edge cases.
class RandomGraph : public Problem {
public:
    [[nodiscard]] State initial() const override { return _initial; }

    [[nodiscard]] State goal() const override { return _goal; }

    [[nodiscard]] bool is_goal(State state) const override { return state == goal(); }

    [[nodiscard]] std::vector<std::shared_ptr<Node>> expand(const std::shared_ptr<Node> &node) const override {
        const auto next_states = _graph.at(node->state());
        std::vector<std::shared_ptr<Node>> expanded_nodes;
        expanded_nodes.reserve(next_states.size());

        const auto new_cost = node->path_cost() + 1;
        for (const auto &ns: next_states) {
            expanded_nodes.push_back(make_shared<Node>(ns, node, new_cost));
        }

        return expanded_nodes;
    }

private:
    RandomGraph(State initial, State goal, std::unordered_map<State, std::unordered_set<State>> graph)
            : _initial{initial}, _goal{goal}, _graph{std::move(graph)} {}

    const State _initial;
    const State _goal;
    const std::unordered_map<State, std::unordered_set<State>> _graph;

    friend RandomGraphFactory; // Allow factory to access private constructor
};


#endif //PARALLEL_BFS_RANDOM_GRAPH_H
