//
// Created by Alejandro Fernández on 19/11/2023.
//

#ifndef PARALLEL_BFS_BASIC_GRAPH_H
#define PARALLEL_BFS_BASIC_GRAPH_H

#include <cstdint>
#include <unordered_map>
#include <unordered_set>

class BasicGraph : public TransitionModel<uint32_t, uint32_t> {
public:
    using graph_t = std::unordered_map<uint32_t, std::unordered_set<uint32_t>>;

    explicit BasicGraph(graph_t &&graph) : _graph{std::move(graph)} {}

protected:
    [[nodiscard]] std::vector<uint32_t> actions(const uint32_t &state) const override {
        auto states = _graph.at(state);
        return {states.cbegin(), states.cend()};
    }

    [[nodiscard]] int action_cost(const uint32_t &current, const uint32_t &action, const uint32_t &next) const override {
        return 1;
    }

    [[nodiscard]] uint32_t result(const uint32_t &state, const uint32_t &action) const override {
        return action;
    }

private:
    const graph_t _graph;
};

#endif //PARALLEL_BFS_BASIC_GRAPH_H
