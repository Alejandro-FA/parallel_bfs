//
// Created by alejandro on 18/11/23.
//

#ifndef PARALLEL_BFS_RANDOM_TREE_FACTORY_H
#define PARALLEL_BFS_RANDOM_TREE_FACTORY_H

#include <queue>
#include <algorithm>
#include "random_problem_factory.h"
#include "../problem/graph_problem.h"
#include "../state/tree_state.h"

class RandomTreeFactory : public RandomProblemFactory<TreeState> {
public:
    /// Creates a random tree with max_depth and a maximum branching factor of max_actions. The average branching
    /// factor is specified by avg_actions, which should be a value between 0 and max_actions
    explicit RandomTreeFactory(state_t max_depth, state_t max_actions, double avg_actions, std::optional<unsigned int> seed = std::nullopt)
            : RandomProblemFactory(seed), _max_depth{max_depth}, _max_actions{max_actions}, _possible_actions(_max_actions),
            _bino_dist{max_actions, avg_actions / max_actions}, _udist{0, max_actions - 1} {
        std::iota(_possible_actions.begin(), _possible_actions.end(), 0);
    }

    /// Builds an Adjacency List representation of a tree.
    [[nodiscard]] std::unique_ptr<Problem<TreeState>> make_problem() override {
        // Create initial state
        TreeState initial{};

        // Create goal state
        std::vector<state_t> goal_vector(_max_actions);
        for (auto &elem : goal_vector) elem = _udist(_prng_engine);
        TreeState goal{std::move(goal_vector)};

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
            for (auto action : get_rand_actions()) {
                TreeState child{state, action};
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
    std::uniform_int_distribution<state_t> _udist;
    std::binomial_distribution<state_t> _bino_dist;
    std::vector<state_t> _possible_actions;

    [[nodiscard]] std::vector<state_t> get_rand_actions() {
        // Decide how many elements to generate
        state_t n{_bino_dist(_prng_engine)};

        // Return n distinct random actions
        std::ranges::shuffle(_possible_actions, _prng_engine);
        return {_possible_actions.cbegin(), _possible_actions.cbegin() + n};
    }
};

#endif //PARALLEL_BFS_RANDOM_TREE_FACTORY_H
