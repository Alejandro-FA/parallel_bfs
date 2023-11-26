//
// Created by Alejandro Fernández on 04/11/2023.
//

#ifndef PARALLEL_BFS_TREE_STATE_H
#define PARALLEL_BFS_TREE_STATE_H

#include <iostream>
#include <vector>
#include "state.h"

typedef uint32_t value_t;

class TreeState final : public State {
public:
    TreeState(std::initializer_list<value_t> init_values = {}) : _vec{init_values} {}

protected:
    const std::vector<value_t> _vec;

    [[nodiscard]] std::ostream &print(std::ostream &os) const override {
        os << "[";
        bool first = true;
        for (const auto &action: _vec) {
            if (!first) os << ", ";
            else first = false;
            os << action;
        }
        return os << "]";
    }

    [[nodiscard]] bool is_equal(const State &rhs) const override {
        if (const auto* p = dynamic_cast<const TreeState*>(&rhs)) return _vec == p->_vec;
        return false;
    }

    /// Algorithm taken from https://stackoverflow.com/a/72073933
    /// In theory it is not needed for this project, but it has been added for completeness.
    [[nodiscard]] std::size_t hash() const override {
        std::size_t seed = _vec.size();
        for(uint32_t x : _vec) { // Perhaps it does not work for other types
            x = ((x >> 16) ^ x) * 0x45d9f3b;
            x = ((x >> 16) ^ x) * 0x45d9f3b;
            x = (x >> 16) ^ x;
            seed ^= x + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        }
        return seed;
    }
};

#endif //PARALLEL_BFS_TREE_STATE_H
