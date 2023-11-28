//
// Created by Alejandro Fernández on 04/11/2023.
//

#ifndef PARALLEL_BFS_TREE_STATE_H
#define PARALLEL_BFS_TREE_STATE_H

#include <iostream>
#include <vector>

using state_t = uint32_t;

class TreeState {
public:
    TreeState(std::initializer_list<state_t> init_values = {}) : _vec{init_values} {}

    explicit TreeState(std::vector<state_t> &&init_values) : _vec{std::move(init_values)} {}

    explicit TreeState(const TreeState &prev_state, state_t new_element) : _vec{prev_state._vec} {
        _vec.push_back(new_element);
    }

    [[nodiscard]] std::size_t depth() const { return _vec.size(); }

private:
    std::vector<state_t> _vec;

    friend std::ostream &operator<<(std::ostream &os, const TreeState &s);

    friend bool operator==(const TreeState &lhs, const TreeState &rhs);

    friend std::hash<TreeState>;
};

std::ostream &operator<<(std::ostream &os, const TreeState &s) {
    os << "[";
    bool first = true;
    for (const auto &action: s._vec) {
        if (!first) os << ", ";
        else first = false;
        os << action;
    }
    return os << "]";
}

bool operator==(const TreeState &lhs, const TreeState &rhs) {
    return lhs._vec == rhs._vec;
}

bool operator!=(const TreeState &lhs, const TreeState &rhs) { return !(lhs == rhs); }

template<>
struct std::hash<TreeState> {
    std::size_t operator()(const TreeState &s) const noexcept {
        std::size_t seed = s._vec.size();
        for (uint32_t x: s._vec) { // Perhaps it does not work for other types
            x = ((x >> 16) ^ x) * 0x45d9f3b;
            x = ((x >> 16) ^ x) * 0x45d9f3b;
            x = (x >> 16) ^ x;
            seed ^= x + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        }
        return seed;
    }
};


#endif //PARALLEL_BFS_TREE_STATE_H
