//
// Created by Alejandro Fernández on 04/11/2023.
//

#ifndef PARALLEL_BFS_TREE_STATE_H
#define PARALLEL_BFS_TREE_STATE_H

#include <cstdint>
#include <iostream>
#include <vector>
#include "../common.h"

template<UnsignedInteger T>
class TreeState {
public:
    TreeState(std::initializer_list<T> init_values = {}) : _vec{init_values} {}

    explicit TreeState(std::vector<T> &&init_values) : _vec{std::move(init_values)} {}

    explicit TreeState(const TreeState &prev_state, T new_element) : _vec{prev_state._vec} {
        _vec.push_back(new_element);
    }

    [[nodiscard]] std::size_t depth() const { return _vec.size(); }

    [[nodiscard]] const std::vector<T> &path() const { return _vec; }

private:
    std::vector<T> _vec;
};


template<UnsignedInteger T>
std::ostream &operator<<(std::ostream &os, const TreeState<T> &s) {
    os << "[";
    bool first = true;
    for (const auto action: s.path()) {
        if (!first) os << ", ";
        else first = false;
        os << action;
    }
    return os << "]";
}


template<UnsignedInteger T>
bool operator==(const TreeState<T> &lhs, const TreeState<T> &rhs) {
    if (lhs.depth() != rhs.depth()) return false;
    return lhs.path() == rhs.path();
}


template<UnsignedInteger T>
bool operator!=(const TreeState<T> &lhs, const TreeState<T> &rhs) { return !(lhs == rhs); }


template<>
struct std::hash<TreeState<std::uint32_t>> {
    /// Algorithm to compute the hash of a vector taken from https://stackoverflow.com/a/72073933
    std::size_t operator()(const TreeState<std::uint32_t> &s) const noexcept {
        std::size_t seed = s.depth();
        for (std::uint32_t x: s.path()) {
            x = ((x >> 16) ^ x) * 0x45d9f3b;
            x = ((x >> 16) ^ x) * 0x45d9f3b;
            x = (x >> 16) ^ x;
            seed ^= x + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        }
        return seed;
    }
};


namespace YAML {
    // Template specialization for std::unordered_set<T>. Needed for problem.goal_states().
    template<parallel_bfs::ConvertibleToYAML T>
    struct convert<TreeState<T>> {
        static Node encode(const TreeState<T> &rhs) {
            Node node(NodeType::Sequence);
            node = rhs.path();
            // std::for_each(rhs.cbegin(), rhs.cend(), [&node](const T &v) { node.push_back(v); });
            node.SetStyle(YAML::EmitterStyle::Flow);
            return node;
        }

        static bool decode(const Node &node, TreeState<T> &rhs) {
            if (!node.IsSequence()) return false;
            std::vector<T> vec = node.as<std::vector<T>>();
            // std::for_each(node.begin(), node.end(), [&rhs](const auto &v) { rhs.insert(v.template as<T>()); });
            rhs = TreeState<T>{std::move(vec)};
            return true;
        }
    };
}


#endif //PARALLEL_BFS_TREE_STATE_H
