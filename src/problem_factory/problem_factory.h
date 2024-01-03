//
// Created by alejandro on 18/11/23.
//

#ifndef PARALLEL_BFS_PROBLEM_FACTORY_H
#define PARALLEL_BFS_PROBLEM_FACTORY_H

template<typename T>
class ProblemFactory {
public:
    virtual ~ProblemFactory() = default;

    [[nodiscard]] std::unique_ptr<Problem<T>> make_problem() {
        return std::make_unique<Problem<T>>(make_initial(), make_goal(), make_child_generator());
    }

protected:
    [[nodiscard]] virtual T make_initial() = 0;

    [[nodiscard]] virtual std::unordered_set<T> make_goal() = 0;

    [[nodiscard]] virtual std::unique_ptr<ChildGenerator<T>> make_child_generator() = 0;
};

#endif //PARALLEL_BFS_PROBLEM_FACTORY_H
