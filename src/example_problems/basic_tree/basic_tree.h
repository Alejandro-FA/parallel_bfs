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

class BasicTree : public parallel_bfs::TransitionModel<TreeState, uint32_t> {
public:
    [[nodiscard]] std::vector<uint32_t> actions(const TreeState &state) const override {
        auto states = _tree.at(state);
        return {states.cbegin(), states.cend()};
    }

    [[nodiscard]] int action_cost(const TreeState &current, const uint32_t &action, const TreeState &next) const override {
        return 1;
    }

    [[nodiscard]] TreeState result(const TreeState &state, const uint32_t &action) const override {
        return TreeState{state, action};
    }

    using tree_t = std::unordered_map<TreeState, std::unordered_set<uint32_t>>;

    explicit BasicTree(tree_t &&tree) : _tree{std::move(tree)} {}

private:
    const tree_t _tree;
};

#endif //PARALLEL_BFS_BASIC_TREE_H
