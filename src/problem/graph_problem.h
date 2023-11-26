//
// Created by Alejandro Fernández on 19/11/2023.
//

#ifndef PARALLEL_BFS_GRAPH_PROBLEM_H
#define PARALLEL_BFS_GRAPH_PROBLEM_H

#include "problem.h"

typedef unordered_map_ptr<State, unordered_set_ptr<State>> graph_t;

/// Class to store Problems that can be represented as graphs.
class GraphProblem : public Problem {
public:
    GraphProblem(std::shared_ptr<State> initial, std::shared_ptr<State> goal, graph_t &&graph)
            : Problem{std::move(initial), std::move(goal)}, _graph{std::move(graph)} {}

    [[nodiscard]] std::vector<std::shared_ptr<State>> next_states_from(const std::shared_ptr<State> &state) const override {
        auto states = _graph.at(state);
        return {states.cbegin(), states.cend()};
    }

private:
    graph_t _graph;
};

#endif //PARALLEL_BFS_GRAPH_PROBLEM_H
