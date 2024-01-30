//
// Created by Alejandro Fernández on 04/11/2023.
//

#ifndef PARALLEL_BFS_NODE_H
#define PARALLEL_BFS_NODE_H

#include <memory>
#include "state.h"

namespace parallel_bfs {
    template<Searchable State>
    class Node {
    public:
        explicit Node(State state, std::shared_ptr<Node<State>> parent = nullptr, int path_cost = 0)
                : _state{std::move(state)}, _parent{std::move(parent)}, _path_cost{path_cost} {}

        [[nodiscard]] State state() const { return _state; }

        [[nodiscard]] std::shared_ptr<Node<State>> parent() const { return _parent; }

        [[nodiscard]] int path_cost() const { return _path_cost; }

    private:
        const State _state;
        const std::shared_ptr<Node<State>> _parent;
        const int _path_cost;
    };
}

#endif //PARALLEL_BFS_NODE_H
