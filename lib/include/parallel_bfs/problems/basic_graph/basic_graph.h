//
// Created by Alejandro Fernández on 19/11/2023.
//

#ifndef PARALLEL_BFS_PROJECT_BASIC_GRAPH_H
#define PARALLEL_BFS_PROJECT_BASIC_GRAPH_H

#include <vector>
#include <unordered_set>
#include <numeric>
#include "../problems_common.h"


namespace parallel_bfs {
    template<detail::UnsignedInteger T>
    class BasicGraph : public TransitionModel<T, T> {
        using graph_t = std::vector<std::unordered_set<T>>;

    public:
        [[nodiscard]] std::vector<T> actions(const T &state) const override {
            auto states = _graph.at(state);
            return {states.cbegin(), states.cend()};
        }

        [[nodiscard]] int action_cost([[maybe_unused]] const T &current, [[maybe_unused]] const T &action, [[maybe_unused]] const T &next) const override {
            return 1;
        }

        [[nodiscard]] T result([[maybe_unused]] const T &state, const T &action) const override {
            return action;
        }

        explicit BasicGraph() = default;

        explicit BasicGraph(T graph_size) : _graph(graph_size) {}

        void push_back(std::unordered_set<T> node) {
            if (size() >= std::numeric_limits<T>::max())
                throw std::overflow_error("Cannot add more nodes to the graph.");
            _graph.push_back(std::move(node));
        }

        std::unordered_set<T> &operator[](std::size_t idx) { return _graph[idx]; }

        const std::unordered_set<T> &operator[](std::size_t idx) const { return _graph[idx]; }

        [[nodiscard]] std::size_t size() const { return _graph.size(); }

        void resize(std::size_t new_size) { _graph.resize(new_size); }

        [[nodiscard]] typename graph_t::iterator begin() { return _graph.begin(); }

        [[nodiscard]] typename graph_t::const_iterator begin() const { return _graph.begin(); }

        [[nodiscard]] typename graph_t::iterator end() { return _graph.end(); }

        [[nodiscard]] typename graph_t::const_iterator end() const { return _graph.end(); }

        [[nodiscard]] std::size_t max_out_degree() const {
            return std::transform_reduce(
                    _graph.cbegin(), _graph.cend(),
                    0,
                    [](std::size_t acc, std::size_t s) { return std::max(acc, s); },
                    [](const std::unordered_set<T> &s) { return s.size(); }
            );
        }

        [[nodiscard]] std::size_t min_out_degree() const {
            return std::transform_reduce(
                    _graph.cbegin(), _graph.cend(),
                    std::numeric_limits<std::size_t>::max(),
                    [](std::size_t acc, std::size_t s) { return std::min(acc, s); },
                    [](const std::unordered_set<T> &s) { return s.size(); }
            );
        }

        [[nodiscard]] double avg_out_degree() const {
            std::size_t sum = std::transform_reduce(
                    _graph.cbegin(), _graph.cend(),
                    0,
                    std::plus<>(),
                    [](const std::unordered_set<T> &s) { return s.size(); }
            );
            return static_cast<double>(sum) / _graph.size();
        }

    private:
        graph_t _graph;
    };
}



namespace YAML {
    template<parallel_bfs::ConvertibleToYAML T>
    struct convert<parallel_bfs::BasicGraph<T>> {
        static Node encode(const parallel_bfs::BasicGraph<T> &rhs) {
            Node output_node(NodeType::Map);
            Node graph_nodes(NodeType::Map);

            for (size_t i = 0; i < rhs.size(); ++i) {
                Node graph_edges(NodeType::Sequence);
                graph_edges = rhs[i];
                graph_nodes[i] = graph_edges;
                graph_edges.SetStyle(YAML::EmitterStyle::Flow);
            }

            output_node["max out-degree"] = rhs.max_out_degree();
            output_node["min out-degree"] = rhs.min_out_degree();
            output_node["avg out-degree"] = rhs.avg_out_degree();
            output_node["graph"] = graph_nodes;
            return output_node;
        }

        static bool decode(const Node &node, parallel_bfs::BasicGraph <T> &rhs) {
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


    template<parallel_bfs::ConvertibleToYAML T>
    Emitter &operator<<(Emitter &out, const parallel_bfs::BasicGraph<T> graph) {
        out << YAML::BeginMap;
        out << YAML::Key << "max out-degree" << YAML::Value << graph.max_out_degree();
        out << YAML::Key << "min out-degree" << YAML::Value << graph.min_out_degree();
        out << YAML::Key << "avg out-degree" << YAML::Value << graph.avg_out_degree();
        out << YAML::Key << "graph" << YAML::Value << YAML::BeginMap;

        for (size_t i = 0; i < graph.size(); ++i)
            out << YAML::Key << i << YAML::Value << YAML::Flow << graph[i];

        out << YAML::EndMap << YAML::EndMap;
        return out;
    }
}

#endif //PARALLEL_BFS_BASIC_GRAPH_H
