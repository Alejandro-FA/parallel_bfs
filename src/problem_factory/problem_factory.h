//
// Created by alejandro on 18/11/23.
//

#ifndef PARALLEL_BFS_PROBLEM_FACTORY_H
#define PARALLEL_BFS_PROBLEM_FACTORY_H

#include <memory>
#include <filesystem>

template<typename T>
class ProblemFactory {
public:
    virtual ~ProblemFactory() = default;

    [[nodiscard]] std::unique_ptr<Problem<T>> make_problem() {
        return std::make_unique<Problem<T>>(get_initial(), get_goal(), get_transition_model());
    }

protected:
    [[nodiscard]] virtual T get_initial() = 0;

    [[nodiscard]] virtual std::unordered_set<T> get_goal() = 0;

    [[nodiscard]] virtual std::unique_ptr<BaseTransitionModel<T>> get_transition_model() = 0;
};

#endif //PARALLEL_BFS_PROBLEM_FACTORY_H
