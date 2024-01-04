//
// Created by Alejandro Fernández on 04/11/2023.
//

#ifndef PARALLEL_BFS_TREE_STATE_H
#define PARALLEL_BFS_TREE_STATE_H

#include <cstdint>
#include <iostream>
#include <vector>

class TreeState {
public:
    TreeState(std::initializer_list<uint32_t> init_values = {}) : _vec{init_values} {}

    explicit TreeState(std::vector<uint32_t> &&init_values) : _vec{std::move(init_values)} {}

    explicit TreeState(const TreeState &prev_state, uint32_t new_element) : _vec{prev_state._vec} {
        _vec.push_back(new_element);
    }

    [[nodiscard]] std::size_t depth() const { return _vec.size(); }

    [[nodiscard]] std::vector<uint32_t> path() const { return _vec; }

private:
    std::vector<uint32_t> _vec;
};


std::ostream &operator<<(std::ostream &os, const TreeState &s) {
    os << "[";
    bool first = true;
    for (const auto action: s.path()) {
        if (!first) os << ", ";
        else first = false;
        os << action;
    }
    return os << "]";
}

bool operator==(const TreeState &lhs, const TreeState &rhs) {
    return lhs.path() == rhs.path();
}

bool operator!=(const TreeState &lhs, const TreeState &rhs) { return !(lhs == rhs); }

template<>
struct std::hash<TreeState> {
    /// Algorithm to compute the hash of a vector taken from https://stackoverflow.com/a/72073933
    std::size_t operator()(const TreeState &s) const noexcept {
        std::size_t seed = s.depth();
        for (uint32_t x: s.path()) {
            x = ((x >> 16) ^ x) * 0x45d9f3b;
            x = ((x >> 16) ^ x) * 0x45d9f3b;
            x = (x >> 16) ^ x;
            seed ^= x + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        }
        return seed;
    }
};


#endif //PARALLEL_BFS_TREE_STATE_H
