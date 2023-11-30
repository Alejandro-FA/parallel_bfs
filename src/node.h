//
// Created by Alejandro Fernández on 04/11/2023.
//

#ifndef PARALLEL_BFS_NODE_H
#define PARALLEL_BFS_NODE_H

#include "state/state.h"

template<Searchable T>
class Node {
public:
    explicit Node(T &&state, std::shared_ptr<Node<T>> parent = nullptr, int path_cost = 0)
            : _state{std::move(state)}, _parent{std::move(parent)}, _path_cost{path_cost} {}

    [[nodiscard]] T state() const { return _state; }

    [[nodiscard]] std::shared_ptr<Node<T>> parent() const { return _parent; }

    [[nodiscard]] int path_cost() const { return _path_cost; }

private:
    const T _state;
    const std::shared_ptr<Node<T>> _parent;
    const int _path_cost;
};

#endif //PARALLEL_BFS_NODE_H
