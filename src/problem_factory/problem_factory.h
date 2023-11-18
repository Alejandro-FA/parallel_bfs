//
// Created by alejandro on 18/11/23.
//

#ifndef PARALLEL_BFS_PROBLEM_FACTORY_H
#define PARALLEL_BFS_PROBLEM_FACTORY_H

#include <random>
#include <optional>
#include "../problem/problem.h"

class ProblemFactory {
public:
    virtual ~ProblemFactory() = default;

    [[nodiscard]] virtual std::unique_ptr<Problem> make_problem() = 0;
};


class RandomProblemFactory : public ProblemFactory {
protected:
    explicit RandomProblemFactory(std::optional<unsigned int> seed) : _prng_engine{get_prng_engine(seed)} {}

    std::default_random_engine _prng_engine;

private:
    static std::default_random_engine get_prng_engine(std::optional<unsigned int> seed) {
        if (seed.has_value()) return std::default_random_engine{seed.value()};
        std::random_device r;
        std::seed_seq seed2{r(), r(), r(), r(), r(), r(), r(), r()};
        return std::default_random_engine{seed2};
    }
};

#endif //PARALLEL_BFS_PROBLEM_FACTORY_H
