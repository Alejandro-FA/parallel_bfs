//
// Created by alejandro on 18/11/23.
//

#ifndef PARALLEL_BFS_RANDOM_TREE_FACTORY_H
#define PARALLEL_BFS_RANDOM_TREE_FACTORY_H

#include <queue>
#include "../../state/tree_state.h"
#include "../../problem/child_generators/tree_generator.h"

class RandomTreeFactory : public RandomProblemFactory<TreeState> {
public:
    /// Creates a random tree with max_depth and a maximum branching factor of max_actions. The average branching
    /// factor is specified by avg_actions, which should be a value between 0 and max_actions
    explicit RandomTreeFactory(unsigned int max_depth, state_t max_actions, double avg_actions,
                               unsigned int num_goals, std::optional<unsigned int> seed = std::nullopt)
            : RandomProblemFactory(seed), _max_depth{max_depth}, _max_actions{max_actions}, _num_goals{num_goals},
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

    /// Builds an Adjacency List representation of a tree.
    [[nodiscard]] std::unique_ptr<ChildGenerator<TreeState>> make_child_generator() override {
        // Create an empty graph
        GraphReader<TreeState>::graph_t g;
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

        return std::make_unique<GraphReader<TreeState>>(std::move(g));
    }

private:
    const unsigned int _max_depth;
    const state_t _max_actions;
    const unsigned int _num_goals;
    TreeGenerator _generator;
    std::binomial_distribution<unsigned int> _bino_dist{_max_depth, 0.9};
};

#endif //PARALLEL_BFS_RANDOM_TREE_FACTORY_H
