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
    explicit ProceduralTreeFactory(state_t max_depth, state_t max_actions, double avg_actions,
                                   std::optional<unsigned int> seed = std::nullopt)
            : RandomProblemFactory(seed), _max_depth{max_depth},
            _generator{max_depth, max_actions, avg_actions, _prng_engine} {}

    /// Creates a ProceduralProblem with randomly generated TreeStates
    [[nodiscard]] std::unique_ptr<Problem<TreeState>> make_problem() override {
        TreeState initial{};
        TreeState goal{};
        while (goal.depth() < _max_depth) goal = _generator(goal)[0];
        auto gen = std::make_unique<TreeStateGenerator>(_generator);
        return std::make_unique<ProceduralProblem<TreeState, TreeStateGenerator>>(
                std::move(initial),
                std::move(goal),
                std::move(gen)
        );
    }

private:
    const state_t _max_depth;
    TreeStateGenerator _generator;
};

#endif //PARALLEL_BFS_PROCEDURAL_TREE_FACTORY_H
