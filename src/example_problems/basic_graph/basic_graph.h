//
// Created by Alejandro Fernández on 19/11/2023.
//

#ifndef PARALLEL_BFS_BASIC_GRAPH_H
#define PARALLEL_BFS_BASIC_GRAPH_H

#include <vector>
#include <unordered_set>
#include <parallel_bfs/search.h>

template<typename T>
concept UnsignedInteger = std::same_as<T, unsigned short> || std::same_as<T, unsigned int> ||
                          std::same_as<T, unsigned long> || std::same_as<T, unsigned long long>;


template<UnsignedInteger T>
class BasicGraph : public parallel_bfs::TransitionModel<T, T> {
public:
    [[nodiscard]] std::vector<T> actions(const T &state) const override {
        auto states = _graph.at(state);
        return {states.cbegin(), states.cend()};
    }

    [[nodiscard]] int action_cost(const T &current, const T &action, const T &next) const override {
        return 1;
    }

    [[nodiscard]] T result(const T &state, const T &action) const override {
        return action;
    }

    using graph_t = std::vector<std::unordered_set<T>>;

    explicit BasicGraph(graph_t &&graph) : _graph{std::move(graph)} {}

    void set_graph(graph_t &&graph) { _graph = std::move(graph); }

    const graph_t &get_graph() const { return _graph; }

private:
    graph_t _graph;
};


namespace YAML {
    template<parallel_bfs::ConvertibleToYAML T>
    struct convert<BasicGraph<T>> {
    static Node encode(const BasicGraph <T> &rhs) {
        const auto graph = rhs.get_graph();
        Node node;
        for(size_t i = 0; i < graph.size(); ++i) {
            node["graph"][i] = graph[i];
        }
        return node;
    }

    static bool decode(const Node &node, BasicGraph <T> &rhs) {
        if (!node.IsSequence()) return false;
        // std::for_each(node.begin(), node.end(), [&rhs](const auto &v) { rhs.insert(v.template as<T>()); });
        std::cout << "WARNING: Unimplemented decoding graph" << std::endl;
        return true;
    }
    };
}

#endif //PARALLEL_BFS_BASIC_GRAPH_H
