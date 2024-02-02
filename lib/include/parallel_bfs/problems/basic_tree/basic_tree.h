//
// Created by alejandro on 04/01/24.
//

#ifndef PARALLEL_BFS_PROJECT_BASIC_TREE_H
#define PARALLEL_BFS_PROJECT_BASIC_TREE_H

#include <unordered_map>
#include <unordered_set>
#include <numeric>
#include "tree_state.h"


namespace parallel_bfs {
    template<detail::UnsignedInteger T>
    class BasicTree : public TransitionModel<TreeState<T>, T> {
        using tree_t = std::unordered_map<TreeState<T>, std::unordered_set<T>>;

    public:
        [[nodiscard]] std::vector<T> actions(const TreeState<T> &state) const override {
            auto states = _tree.at(state);
            return {states.cbegin(), states.cend()};
        }

        [[nodiscard]] int
        action_cost([[maybe_unused]] const TreeState<T> &current, [[maybe_unused]] const T &action, [[maybe_unused]] const TreeState<T> &next) const override {
            return 1;
        }

        [[nodiscard]] TreeState<T> result(const TreeState<T> &state, const T &action) const override {
            return TreeState{state, action};
        }

        explicit BasicTree() = default;

        explicit BasicTree(T tree_size) { _tree.reserve(tree_size); }

        std::unordered_set<T> &operator[](const TreeState<T> &key) { return _tree[key]; }

        const std::unordered_set<T> &operator[](const TreeState<T> &key) const { return _tree.at(key); }

        void insert(const TreeState<T> &key, std::unordered_set<T> value) { _tree.insert({key, std::move(value)}); }

        void erase(const TreeState<T> &key) { _tree.erase(key); }

        [[nodiscard]] std::size_t size() const { return _tree.size(); }

        void reserve(std::size_t count) { _tree.reserve(count); }

        [[nodiscard]] typename tree_t::iterator begin() { return _tree.begin(); }

        [[nodiscard]] typename tree_t::const_iterator begin() const { return _tree.begin(); }

        [[nodiscard]] typename tree_t::iterator end() { return _tree.end(); }

        [[nodiscard]] typename tree_t::const_iterator end() const { return _tree.end(); }

        [[nodiscard]] std::size_t max_depth() const {
            return std::transform_reduce(
                    _tree.cbegin(), _tree.cend(),
                    0,
                    [](std::size_t acc, std::size_t s) { return std::max(acc, s); },
                    [](const std::pair<TreeState<T>, std::unordered_set<T>> &kv) { return kv.first.depth(); }
            );
        }

        /// Returns the average depth of leaf nodes.
        [[nodiscard]] double avg_depth() const {
            std::size_t sum = std::transform_reduce(
                    _tree.cbegin(), _tree.cend(),
                    0,
                    std::plus<>(),
                    [](const std::pair<TreeState<T>, std::unordered_set<T>> &kv) {
                        return kv.second.size() == 0 ? kv.first.depth() : 0;
                    }
            );
            std::size_t leaf_node_count = std::transform_reduce(
                    _tree.cbegin(), _tree.cend(),
                    0,
                    std::plus<>(),
                    [](const std::pair<TreeState<T>, std::unordered_set<T>> &kv) { return kv.second.size() == 0 ? 1 : 0; }
            );
            return static_cast<double>(sum) / static_cast<double>(leaf_node_count);
        }

        [[nodiscard]] std::size_t max_branch_factor() const {
            return std::transform_reduce(
                    _tree.cbegin(), _tree.cend(),
                    0,
                    [](std::size_t acc, std::size_t s) { return std::max(acc, s); },
                    [](const std::pair<TreeState<T>, std::unordered_set<T>> &kv) { return kv.second.size(); }
            );
        }

        /// Returns the average branching factor of internal nodes.
        [[nodiscard]] double avg_branch_factor() const {
            std::size_t sum = std::transform_reduce(
                    _tree.cbegin(), _tree.cend(),
                    0,
                    std::plus<>(),
                    [](const std::pair<TreeState<T>, std::unordered_set<T>> &kv) { return kv.second.size(); }
            );
            std::size_t internal_node_count = std::transform_reduce(
                    _tree.cbegin(), _tree.cend(),
                    0,
                    std::plus<>(),
                    [](const std::pair<TreeState<T>, std::unordered_set<T>> &kv) { return kv.second.size() == 0 ? 0 : 1; }
            );
            return static_cast<double>(sum) / static_cast<double>(internal_node_count);
        }

    private:
        tree_t _tree;
    };
}


namespace YAML {
    template<parallel_bfs::ConvertibleToYAML T>
    struct convert<parallel_bfs::BasicTree<T>> {
        static Node encode(const parallel_bfs::BasicTree <T> &rhs) {
            Node output_node(NodeType::Map);
            Node tree_nodes(NodeType::Map);

            for (const auto &[key, value]: rhs) {
                Node tree_children(NodeType::Sequence);
                tree_children = value;
                tree_nodes[key] = tree_children;
                tree_children.SetStyle(YAML::EmitterStyle::Flow);
            }

            output_node["max depth"] = rhs.max_depth();
            output_node["avg depth"] = rhs.avg_depth();
            output_node["max branch factor"] = rhs.max_branch_factor();
            output_node["avg branch factor"] = rhs.avg_branch_factor();
            output_node["tree"] = tree_nodes;
            return output_node;
        }

        static bool decode(const Node &node, parallel_bfs::BasicTree<T> &rhs) {
            if (!node.IsMap()) return false;
            Node tree_node = node["tree"];
            if (!tree_node.IsMap()) return false;
            rhs.reserve(tree_node.size());

            for (const auto key_value_pair: tree_node) {
                parallel_bfs::TreeState<T> state = key_value_pair.first.template as<parallel_bfs::TreeState<T>>();
                std::unordered_set<T> children = key_value_pair.second.template as<std::unordered_set<T>>();
                rhs.insert(state, std::move(children));
            }

            return true;
        }
    };


    template<parallel_bfs::ConvertibleToYAML T>
    Emitter &operator<<(Emitter &out, const parallel_bfs::BasicTree<T> tree) {
        out << YAML::BeginMap;
        out << YAML::Key << "max depth" << YAML::Value << tree.max_depth();
        out << YAML::Key << "avg depth" << YAML::Value << tree.avg_depth();
        out << YAML::Key << "max branch factor" << YAML::Value << tree.max_branch_factor();
        out << YAML::Key << "avg branch factor" << YAML::Value << tree.avg_branch_factor();
        out << YAML::Key << "tree" << YAML::Value << YAML::BeginMap;

        for (const auto &[key, value]: tree)
            out << YAML::Key << key << YAML::Value << YAML::Flow << value;

        out << YAML::EndMap << YAML::EndMap;
        return out;
    }
}

#endif //PARALLEL_BFS_PROJECT_BASIC_TREE_H
