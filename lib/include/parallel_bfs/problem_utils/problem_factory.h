//
// Created by alejandro on 18/11/23.
//

#ifndef PARALLEL_BFS_PROBLEM_FACTORY_H
#define PARALLEL_BFS_PROBLEM_FACTORY_H

#include <memory>
#include <unordered_set>
#include <random>
#include <optional>
#include <ranges>


template<typename T>
concept RandomDist = requires(T &dist, std::default_random_engine &engine) {
    typename T::result_type;  // This requires T to have a type named result_type
    { dist(engine) } -> std::same_as<typename T::result_type>;
};



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
    public:
        [[nodiscard]] Problem<State, TM> make_problem(std::optional<unsigned int> seed = std::nullopt) {
            set_random_engine(seed);
            return ProblemFactory<State, TM>::make_problem();
        }

    protected:
        template<RandomDist Dist>
        [[nodiscard]] typename Dist::result_type get_random_value(Dist &random_dist) {
            return random_dist(_prng_engine);
        }

        template<RandomDist Dist>
        [[nodiscard]] std::vector<typename Dist::result_type> get_random_values(Dist &random_dist, unsigned int n) {
            std::vector<typename Dist::result_type> output(n);
            for (auto &v: output) v = get_random_value(random_dist);
            std::generate(output.begin(), output.end(), [&] { return get_random_value(random_dist); });
            return output;
        }

        /// Returns a random sample of size n from the vector r. The order of the elements is not guaranteed to be
        /// preserved nor is it guaranteed to be modified.
        template<std::ranges::sized_range R>
        [[nodiscard]] std::vector<std::ranges::range_value_t<R>> get_random_sample(const R &r, unsigned int n) {
            if (n > std::ranges::size(r)) throw std::invalid_argument("n cannot be larger than the size of the vector.");
            std::vector<std::ranges::range_value_t<R>> output(n);
            std::sample(r.cbegin(), r.cend(), output.begin(), n, _prng_engine);
            return output;
        }

    private:
        void set_random_engine(std::optional<unsigned int> seed) {
            // Only reseed if the seed is specified or if the PRNG was manually seeded before
            if (seed.has_value() or _manually_seeded) _prng_engine = get_random_engine(seed);
            _manually_seeded = seed.has_value();
        }

        static std::default_random_engine get_random_engine(std::optional<unsigned int> seed = std::nullopt) {
            if (seed.has_value()) return std::default_random_engine{seed.value()};
            std::random_device r;
            std::seed_seq seed2{r(), r(), r(), r(), r(), r(), r(), r()};
            return std::default_random_engine{seed2};
        }

        std::default_random_engine _prng_engine {get_random_engine()};
        bool _manually_seeded = false;
    };
}

#endif //PARALLEL_BFS_PROBLEM_FACTORY_H
