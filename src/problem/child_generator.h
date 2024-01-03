//
// Created by Alejandro Fernández on 02/01/2024.
//

#ifndef PARALLEL_BFS_CHILD_GENERATOR_H
#define PARALLEL_BFS_CHILD_GENERATOR_H

#include <vector>

template<typename T>
class ChildGenerator {
public:
    virtual ~ChildGenerator() = default;

    [[nodiscard]] virtual std::vector<T> operator()(const T &state) = 0;
};

#endif //PARALLEL_BFS_CHILD_GENERATOR_H
