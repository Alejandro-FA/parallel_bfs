//
// Created by alejandro on 18/11/23.
//

#ifndef PARALLEL_BFS_PROBLEM_H
#define PARALLEL_BFS_PROBLEM_H

#include <vector>
#include "../state.h"
#include "../node.h"

class Problem {
public:
    virtual ~Problem() = default;

    [[nodiscard]] virtual State initial() const = 0;

    [[nodiscard]] virtual State goal() const = 0;

    [[nodiscard]] virtual bool is_goal(State state) const = 0;

    // TODO: Change this to a coroutine when std::generator (C++23) is implemented in g++
    [[nodiscard]] virtual std::vector<std::shared_ptr<Node>> expand(const std::shared_ptr<Node> &node) const = 0;

    void print() const {
        std::cout << "\nInitial state: " << initial() << "\nGoal state: " << goal() << std::endl;
    }
};

#endif //PARALLEL_BFS_PROBLEM_H
