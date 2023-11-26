//
// Created by Alejandro Fernández on 04/11/2023.
//

#ifndef PARALLEL_BFS_NODE_H
#define PARALLEL_BFS_NODE_H

#include <memory>
#include "state/state.h"

class Node {
public:
    explicit Node(std::shared_ptr<State> state, std::shared_ptr<Node> parent = nullptr, int path_cost = 0)
            : _state{std::move(state)}, _parent{std::move(parent)}, _path_cost{path_cost} {}

    [[nodiscard]] std::shared_ptr<State> state() const { return _state; }

    [[nodiscard]] std::shared_ptr<Node> parent() const { return _parent; }

    [[nodiscard]] int path_cost() const { return _path_cost; }

private:
    const std::shared_ptr<State> _state;
    const std::shared_ptr<Node> _parent;
    const int _path_cost;
};

#endif //PARALLEL_BFS_NODE_H
