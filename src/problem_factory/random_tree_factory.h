//
// Created by alejandro on 18/11/23.
//

#ifndef PARALLEL_BFS_RANDOM_TREE_FACTORY_H
#define PARALLEL_BFS_RANDOM_TREE_FACTORY_H

#include <queue>
#include "random_problem_factory.h"
#include "../problem/graph_problem.h"
#include "../state/tree_state.h"
#include "state_generators/tree_state_generator.h"

class RandomTreeFactory : public RandomProblemFactory<TreeState> {
public:
    /// Creates a random tree with max_depth and a maximum branching factor of max_actions. The average branching
    /// factor is specified by avg_actions, which should be a value between 0 and max_actions
    explicit RandomTreeFactory(state_t max_depth, state_t max_actions, double avg_actions,
                               std::optional<unsigned int> seed = std::nullopt)
            : RandomProblemFactory(seed), _max_depth{max_depth}, _max_actions{max_actions},
              _generator{max_depth, max_actions, avg_actions, _prng_engine} {}

    /// Builds an Adjacency List representation of a tree.
    [[nodiscard]] std::unique_ptr<Problem<TreeState>> make_problem() override {
        TreeState initial{}; // initial state
        TreeState goal{}; // goal state
        while (goal.depth() < _max_depth) goal = _generator(goal)[0];

        // Create an empty graph
        GraphProblem<TreeState>::graph_t g;
        auto max_size = static_cast<std::size_t>(pow(_max_actions, _max_depth));
        g.reserve(max_size);

        // Fill the graph using Breadth-First-Search
        std::queue<TreeState> frontier({TreeState{}});
        g.insert({TreeState{}, std::unordered_set<TreeState>()});

        while (!frontier.empty()) {
            TreeState state{frontier.front()};
            frontier.pop();
            if (state.depth() == _max_depth) break; // Stop once we reach max_depth
            for (const auto &child: _generator(state)) {
                g.insert({child, std::unordered_set<TreeState>()});
                frontier.push(child);
                g[state].insert(child);
            }
        }

        return std::make_unique<GraphProblem<TreeState>>(std::move(initial), std::move(goal), std::move(g));
    }

private:
    const state_t _max_depth;
    const state_t _max_actions;
    TreeStateGenerator _generator;
};

#endif //PARALLEL_BFS_RANDOM_TREE_FACTORY_H
