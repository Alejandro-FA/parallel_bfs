//
// Created by Alejandro Fernández on 29/11/2023.
//

#ifndef PARALLEL_BFS_PROCEDURAL_TREE_FACTORY_H
#define PARALLEL_BFS_PROCEDURAL_TREE_FACTORY_H

#include "random_problem_factory.h"
#include "../problem/procedural_problem.h"
#include "state_generators/tree_state_generator.h"

class ProceduralTreeFactory : public RandomProblemFactory<TreeState> {
public:
    /// Creates a random tree with max_depth and a maximum branching factor of max_actions. The average branching
    /// factor is specified by avg_actions, which should be a value greater than 0 and less or equal than max_actions.
    explicit ProceduralTreeFactory(unsigned int max_depth, state_t max_actions, double avg_actions,
                                   unsigned int num_goals, std::optional<unsigned int> seed = std::nullopt)
            : RandomProblemFactory(seed), _max_depth{max_depth}, _num_goals{num_goals},
            _generator{max_depth, max_actions, avg_actions, _prng_engine} {}

    /// Creates a ProceduralProblem with randomly generated TreeStates
    [[nodiscard]] std::unique_ptr<Problem<TreeState>> make_problem() override {
        TreeState initial{};

        // Generate a bunch of goal states. Not all goal states have the same depth.
        std::unordered_set<TreeState> goals(_num_goals);
        while (goals.size() < _num_goals) {
            TreeState goal{};
            unsigned int goal_depth = _bino_dist(_prng_engine);
            for (auto v = _generator(goal); goal.depth() < goal_depth; v = _generator(v[0]))
                if (!v.empty()) goal = v[0];
            goals.insert(std::move(goal));
        }

        auto gen = std::make_unique<TreeStateGenerator>(_generator);
        return std::make_unique<ProceduralProblem<TreeState, TreeStateGenerator>>(
                std::move(initial),
                std::move(goals),
                std::move(gen)
        );
    }

private:
    const unsigned int _max_depth;
    const unsigned int _num_goals;
    TreeStateGenerator _generator;
    std::binomial_distribution<unsigned int> _bino_dist{_max_depth, 0.9};
};

#endif //PARALLEL_BFS_PROCEDURAL_TREE_FACTORY_H
