//
// Created by alejandro on 18/11/23.
//

#ifndef PARALLEL_BFS_PROBLEM_FACTORY_H
#define PARALLEL_BFS_PROBLEM_FACTORY_H

template<typename T>
class ProblemFactory {
public:
    virtual ~ProblemFactory() = default;

    [[nodiscard]] virtual std::unique_ptr<Problem<T>> make_problem() = 0;
};

#endif //PARALLEL_BFS_PROBLEM_FACTORY_H
