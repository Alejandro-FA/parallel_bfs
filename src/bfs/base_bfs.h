//
// Created by Alejandro Fernández on 06/11/2023.
//

#ifndef PARALLEL_BFS_BASE_BFS_H
#define PARALLEL_BFS_BASE_BFS_H

#include "../problem.h"
#include "../node.h"

class BFS {
public:
    virtual ~BFS() = default;

    [[nodiscard]] virtual std::shared_ptr<Node> operator()(const Problem &problem) const = 0;

    [[nodiscard]] virtual std::string get_name() const = 0;
};

#endif //PARALLEL_BFS_BASE_BFS_H
