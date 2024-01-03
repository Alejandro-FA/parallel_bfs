//
// Created by Alejandro Fernández on 19/11/2023.
//

#ifndef PARALLEL_BFS_GRAPH_READER_H
#define PARALLEL_BFS_GRAPH_READER_H

#include <unordered_map>
#include <unordered_set>

template<typename T>
class GraphReader : public ChildGenerator<T> {
public:
    using graph_t = std::unordered_map<T, std::unordered_set<T>>;

    explicit GraphReader(graph_t &&graph) : _graph{std::move(graph)} {}

    [[nodiscard]] std::vector<T> operator()(const T &state) override {
        auto states = _graph.at(state);
        return {states.cbegin(), states.cend()};
    }

private:
    const graph_t _graph;
};

#endif //PARALLEL_BFS_GRAPH_READER_H
