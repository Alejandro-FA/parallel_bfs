//
// Created by alejandro on 18/11/23.
//

#ifndef PARALLEL_BFS_RANDOM_TREE_FACTORY_H
#define PARALLEL_BFS_RANDOM_TREE_FACTORY_H

#include <optional>
#include <queue>
#include <algorithm>
#include "random_problem_factory.h"
#include "../problem/graph_problem.h"
#include "../state/tree_state.h"

class RandomTreeFactory : public RandomProblemFactory {
public:
    /// Creates a random tree with max_depth and a maximum branching factor of max_actions. The average branching
    /// factor is specified by avg_actions, which should be a value between 0 and max_actions
    explicit RandomTreeFactory(value_t max_depth, value_t max_actions, double avg_actions, std::optional<unsigned int> seed = std::nullopt)
            : RandomProblemFactory(seed), _max_depth{max_depth}, _max_actions{max_actions}, _possible_actions(_max_actions),
            _bino_dist{max_actions, avg_actions / max_actions}, _udist{0, max_actions - 1} {
        std::iota(_possible_actions.begin(), _possible_actions.end(), 0);
    }

    /// Builds an Adjacency List representation of a tree.
    [[nodiscard]] std::unique_ptr<Problem> make_problem() override {
        // Create initial state
        std::shared_ptr<TreeState> initial = std::make_shared<TreeState>();

        // Create goal state
        std::vector<value_t> goal_vector(_max_actions);
        for (auto &elem : goal_vector) elem = _udist(_prng_engine);
        std::shared_ptr<TreeState> goal = std::make_shared<TreeState>(std::move(goal_vector));

        // Create an empty graph
        graph_t g;
        auto max_states = static_cast<std::size_t>(pow(_max_actions, _max_depth));
        g.reserve(max_states);

        // Fill the graph using Breadth-First-Search
        std::queue<std::shared_ptr<TreeState>> frontier({initial});

        while (!frontier.empty()) {
            auto state = frontier.front();
            frontier.pop();
            // With Breadth-First-Search, once we reach max_depth all remaining states also have max_depth
            if (state->depth() == _max_depth) break;
            auto random_actions{get_rand_actions()};
            for (auto action : random_actions) {
                std::shared_ptr<TreeState> child = std::make_shared<TreeState>(*state, action);
                g.insert({child, unordered_set_ptr<State>()});
                frontier.push(child);
                g[state].insert(child);
            }
        }

        return std::make_unique<GraphProblem>(initial, goal, std::move(g));
    }

private:
    const value_t _max_depth;
    const value_t _max_actions;
    std::uniform_int_distribution<value_t> _udist;
    std::binomial_distribution<value_t> _bino_dist;
    std::vector<value_t> _possible_actions;

    [[nodiscard]] std::vector<value_t> get_rand_actions() {
        // Decide how many elements to generate
        value_t n{_bino_dist(_prng_engine)};

        // Return n distinct random actions
        std::ranges::shuffle(_possible_actions, _prng_engine);
        return {_possible_actions.cbegin(), _possible_actions.cbegin() + n};
    }
};

#endif //PARALLEL_BFS_RANDOM_TREE_FACTORY_H
