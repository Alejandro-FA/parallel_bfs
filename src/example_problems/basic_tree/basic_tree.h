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

    using tree_t = std::unordered_map<TreeState<T>, std::unordered_set<T>>;

    explicit BasicTree(tree_t &&tree) : _tree{std::move(tree)} {}

    void set_tree(tree_t &&tree) { _tree = std::move(tree); }

    const tree_t &get_tree() const { return _tree; }

private:
    const tree_t _tree;
};


namespace YAML {
    template<parallel_bfs::ConvertibleToYAML T>
    struct convert<BasicTree<T>> {
        static Node encode(const BasicTree<T> &rhs) {
            const auto tree = rhs.get_tree();
            Node output_node(NodeType::Map);
            Node tree_nodes(NodeType::Map);

            for (const auto& [key, value]: tree) {
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
