//
// Created by Alejandro Fernández on 19/11/2023.
//

#ifndef PARALLEL_BFS_GRAPH_PROBLEM_H
#define PARALLEL_BFS_GRAPH_PROBLEM_H

#include <unordered_map>
#include <unordered_set>
#include "problem.h"

template<typename T>
concept Hashable = requires(const T &a) {
    { std::hash<T>{}(a) } -> std::convertible_to<std::size_t>;
};

/// Class to store Problems that can be represented as graphs.
template<Hashable T>
class GraphProblem : public Problem<T> {
public:
    using graph_t = std::unordered_map<T, std::unordered_set<T>>;

    explicit GraphProblem(T initial, T goal, graph_t &&graph)
            : Problem<T>{std::move(initial), std::move(goal)}, _graph{std::move(graph)} {}

    [[nodiscard]] std::vector<T> next_states_from(const T &state) const override {
        auto states = _graph.at(state);
        return {states.cbegin(), states.cend()};
    }

private:
    const graph_t _graph;
};

#endif //PARALLEL_BFS_GRAPH_PROBLEM_H
