//
// Created by Alejandro Fernández on 04/11/2023.
//

#ifndef PARALLEL_BFS_TREE_STATE_H
#define PARALLEL_BFS_TREE_STATE_H

#include <iostream>
#include <vector>

using state_t = uint32_t;

struct TreeState {
    std::vector<state_t> vec;

    TreeState(std::initializer_list<state_t> init_values = {}) : vec{init_values} {}

    explicit TreeState(std::vector<state_t> &&init_values) : vec{std::move(init_values)} {}

    explicit TreeState(const TreeState &prev_state, state_t new_element) :
            vec{add_element(prev_state.vec, new_element)} {}

    [[nodiscard]] std::size_t depth() const { return vec.size(); }

    static std::vector<state_t> add_element(const std::vector<state_t> &old_vector, state_t new_element) {
        std::vector<state_t> result{old_vector.begin(), old_vector.end()};
        result.push_back(new_element);
        return result;
    }
};

std::ostream &operator<<(std::ostream &os, const TreeState &s) {
    os << "[";
    bool first = true;
    for (const auto &action: s.vec) {
        if (!first) os << ", ";
        else first = false;
        os << action;
    }
    return os << "]";
}

bool operator==(const TreeState &lhs, const TreeState &rhs) {
    return lhs.vec == rhs.vec;
}

bool operator!=(const TreeState &lhs, const TreeState &rhs) { return !(lhs == rhs); }

template<>
struct std::hash<TreeState> {
    std::size_t operator()(const TreeState &s) const noexcept {
        std::size_t seed = s.vec.size();
        for (uint32_t x: s.vec) { // Perhaps it does not work for other types
            x = ((x >> 16) ^ x) * 0x45d9f3b;
            x = ((x >> 16) ^ x) * 0x45d9f3b;
            x = (x >> 16) ^ x;
            seed ^= x + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        }
        return seed;
    }
};


#endif //PARALLEL_BFS_TREE_STATE_H
