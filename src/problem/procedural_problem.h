//
// Created by Alejandro Fernández on 26/11/2023.
//

#ifndef PARALLEL_BFS_PROCEDURAL_PROBLEM_H
#define PARALLEL_BFS_PROCEDURAL_PROBLEM_H

#include "problem.h"

template<typename F, typename T>
concept StateGenerator = requires(F f, const T &state) {
    { f(state) } -> std::convertible_to<std::vector<T>>;
};

template<typename T, StateGenerator<T> Generator>
class ProceduralProblem : public Problem<T> {
public:
    ProceduralProblem(T initial, T goal, std::unique_ptr<Generator> gen)
            : Problem<T>{std::move(initial), std::move(goal)}, _gen{std::move(gen)} {}

    [[nodiscard]] std::vector<T> next_states_from(const T &state) const override {
        return (*_gen)(state);
    }

private:
    const std::unique_ptr<Generator> _gen;
};

#endif //PARALLEL_BFS_PROCEDURAL_PROBLEM_H
