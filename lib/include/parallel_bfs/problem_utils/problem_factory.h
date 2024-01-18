//
// Created by alejandro on 18/11/23.
//

#ifndef PARALLEL_BFS_PROBLEM_FACTORY_H
#define PARALLEL_BFS_PROBLEM_FACTORY_H

#include <memory>
#include <unordered_set>
#include <random>
#include <optional>

namespace parallel_bfs {
    template<State State, std::derived_from<BaseTransitionModel<State>> TM>
    class ProblemFactory {
    public:
        [[nodiscard]] Problem<State, TM> make_problem() {
            return Problem{get_initial(), get_goal_states(), get_transition_model()};
        }

    protected:
        [[nodiscard]] virtual State get_initial() = 0;

        [[nodiscard]] virtual std::unordered_set<State> get_goal_states() = 0;

        [[nodiscard]] virtual TM get_transition_model() = 0;
    };



    template<State State, std::derived_from<BaseTransitionModel<State>> TM>
    class RandomFactory : public ProblemFactory<State, TM> {
    protected:
        explicit RandomFactory(std::optional<unsigned int> seed) : _prng_engine{get_prng_engine(seed)} {}

        std::default_random_engine _prng_engine;

    private:
        static std::default_random_engine get_prng_engine(std::optional<unsigned int> seed) {
            if (seed.has_value()) return std::default_random_engine{seed.value()};
            std::random_device r;
            std::seed_seq seed2{r(), r(), r(), r(), r(), r(), r(), r()};
            return std::default_random_engine{seed2};
        }
    };
}

#endif //PARALLEL_BFS_PROBLEM_FACTORY_H
