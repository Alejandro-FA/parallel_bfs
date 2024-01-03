//
// Created by Alejandro Fernández on 29/11/2023.
//

#ifndef PARALLEL_BFS_PROCEDURAL_TREE_FACTORY_H
#define PARALLEL_BFS_PROCEDURAL_TREE_FACTORY_H

#include "../random_problem_factory.h"
#include "../../problem/child_generators/tree_generator.h"

class ProceduralTreeFactory : public RandomProblemFactory<TreeState> {
public:
    /// Creates a random tree with max_depth and a maximum branching factor of max_actions. The average branching
    /// factor is specified by avg_actions, which should be a value greater than 0 and less or equal than max_actions.
    explicit ProceduralTreeFactory(unsigned int max_depth, state_t max_actions, double avg_actions,
                                   unsigned int num_goals, std::optional<unsigned int> seed = std::nullopt)
            : RandomProblemFactory(seed), _max_depth{max_depth}, _num_goals{num_goals},
            _generator{max_depth, max_actions, avg_actions, _prng_engine} {}


    [[nodiscard]] TreeState make_initial() override { return TreeState{}; }

    [[nodiscard]] std::unordered_set<TreeState> make_goal() override {
        // Generate a bunch of goal states. Not all goal states have the same depth.
        std::unordered_set<TreeState> goals(_num_goals);
        while (goals.size() < _num_goals) {
            TreeState goal{};
            unsigned int goal_depth = _bino_dist(_prng_engine);
            for (auto v = _generator(goal); goal.depth() < goal_depth; v = _generator(v[0]))
                if (!v.empty()) goal = v[0];
            goals.insert(std::move(goal));
        }

        return goals;
    }

    /// Creates a ProceduralProblem with randomly generated TreeStates
    [[nodiscard]] std::unique_ptr<ChildGenerator<TreeState>> make_child_generator() override {
        return std::make_unique<TreeGenerator>(_generator);
    }

private:
    const unsigned int _max_depth;
    const unsigned int _num_goals;
    TreeGenerator _generator;
    std::binomial_distribution<unsigned int> _bino_dist{_max_depth, 0.9};
};

#endif //PARALLEL_BFS_PROCEDURAL_TREE_FACTORY_H
