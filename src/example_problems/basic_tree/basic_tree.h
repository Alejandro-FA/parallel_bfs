//
// Created by alejandro on 04/01/24.
//

#ifndef PARALLEL_BFS_BASIC_TREE_H
#define PARALLEL_BFS_BASIC_TREE_H

#include <cstdint>
#include <unordered_map>
#include <unordered_set>
#include <parallel_bfs/search.h>
#include "tree_state.h"

template<UnsignedInteger T>
class BasicTree : public parallel_bfs::TransitionModel<TreeState<T>, T> {
    using tree_t = std::unordered_map<TreeState<T>, std::unordered_set<T>>;

public:
    [[nodiscard]] std::vector<T> actions(const TreeState<T> &state) const override {
        auto states = _tree.at(state);
        return {states.cbegin(), states.cend()};
    }

    [[nodiscard]] int action_cost(const TreeState<T> &current, const T &action, const TreeState<T> &next) const override {
        return 1;
    }

    [[nodiscard]] TreeState<T> result(const TreeState<T> &state, const T &action) const override {
        return TreeState{state, action};
    }

    explicit BasicTree() = default;

    explicit BasicTree(T tree_size) { _tree.reserve(tree_size); }

    std::unordered_set<T>& operator[](const TreeState<T>& key) { return _tree[key]; }

    const std::unordered_set<T>& operator[](const TreeState<T>& key) const { return _tree.at(key); }

    void insert(const TreeState<T>& key, std::unordered_set<T> value) { _tree.insert({key, std::move(value)}); }

    void erase(const TreeState<T>& key) { _tree.erase(key); }

    [[nodiscard]] std::size_t size() const { return _tree.size(); }

    typename tree_t::iterator begin() { return _tree.begin(); }

    typename tree_t::const_iterator begin() const { return _tree.begin(); }

    typename tree_t::iterator end() { return _tree.end(); }

    typename tree_t::const_iterator end() const { return _tree.end(); }

private:
    tree_t _tree;
};


namespace YAML {
    template<parallel_bfs::ConvertibleToYAML T>
    struct convert<BasicTree<T>> {
        static Node encode(const BasicTree<T> &rhs) {
            Node output_node(NodeType::Map);
            Node tree_nodes(NodeType::Map);

            for (const auto& [key, value]: rhs) {
                Node tree_children(NodeType::Sequence);
                tree_children = value;
                tree_nodes[key] = tree_children;
                tree_children.SetStyle(YAML::EmitterStyle::Flow);
            }

            output_node["tree"] = tree_nodes;
            return output_node;
        }

        static bool decode(const Node &node, BasicTree<T> &rhs) {
            if (!node.IsSequence()) return false;
            // std::for_each(node.begin(), node.end(), [&rhs](const auto &v) { rhs.insert(v.template as<T>()); });
            std::cout << "WARNING: Unimplemented decoding tree" << std::endl;
            return true;
        }
    };
}

#endif //PARALLEL_BFS_BASIC_TREE_H
