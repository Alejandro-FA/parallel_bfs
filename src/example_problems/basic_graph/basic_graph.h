//
// Created by Alejandro Fernández on 19/11/2023.
//

#ifndef PARALLEL_BFS_BASIC_GRAPH_H
#define PARALLEL_BFS_BASIC_GRAPH_H

#include <vector>
#include <unordered_set>
#include <parallel_bfs/search.h>
#include "../common.h"


template<UnsignedInteger T>
class BasicGraph : public parallel_bfs::TransitionModel<T, T> {
    using graph_t = std::vector<std::unordered_set<T>>;

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

    explicit BasicGraph() = default;

    explicit BasicGraph(T graph_size) : _graph(graph_size) {}

    void push_back(std::unordered_set<T> node) { _graph.push_back(std::move(node)); }

    std::unordered_set<T>& operator[](std::size_t idx) { return _graph[idx]; }

    const std::unordered_set<T>& operator[](std::size_t idx) const { return _graph[idx]; }

    [[nodiscard]] std::size_t size() const { return _graph.size(); }

    void resize(std::size_t new_size) { _graph.resize(new_size); }

    typename graph_t::iterator begin() { return _graph.begin(); }

    typename graph_t::const_iterator begin() const { return _graph.begin(); }

    typename graph_t::iterator end() { return _graph.end(); }

    typename graph_t::const_iterator end() const { return _graph.end(); }

private:
    graph_t _graph;
};


namespace YAML {
    template<parallel_bfs::ConvertibleToYAML T>
    struct convert<BasicGraph<T>> {
        static Node encode(const BasicGraph<T> &rhs) {
            Node output_node(NodeType::Map);
            Node graph_nodes(NodeType::Map);

            for(size_t i = 0; i < rhs.size(); ++i) {
                Node graph_edges(NodeType::Sequence);
                graph_edges = rhs[i];
                graph_nodes[i] = graph_edges;
                graph_edges.SetStyle(YAML::EmitterStyle::Flow);
            }

            output_node["graph"] = graph_nodes;
            return output_node;
        }

        static bool decode(const Node &node, BasicGraph<T> &rhs) {
            if (!node.IsMap()) return false;
            Node graph_node = node["graph"];
            if (!graph_node.IsMap()) return false;
            rhs.resize(graph_node.size());

            for (const auto key_value_pair: graph_node) {
                T node_idx = key_value_pair.first.template as<T>();
                std::unordered_set<T> children = key_value_pair.second.template as<std::unordered_set<T>>();
                rhs[node_idx] = std::move(children);
            }

            return true;
        }
    };
}

#endif //PARALLEL_BFS_BASIC_GRAPH_H
