//
// Created by Alejandro Fernández on 29/11/2023.
//

#ifndef PARALLEL_BFS_BASIC_TREE_BUILDER_H
#define PARALLEL_BFS_BASIC_TREE_BUILDER_H

#include <queue>
#include <algorithm>
#include "../../state/tree_state.h"
#include "../../problem/transition_models/basic_tree.h"

class BasicTreeBuilder : public RandomBuilder<TreeState> {
public:
    /// Creates a random tree with max_depth and a maximum branching factor of max_actions. The average branching
    /// factor is specified by avg_actions, which should be a value between 0 and max_actions
    explicit BasicTreeBuilder(unsigned int max_depth, unsigned int num_goals, unsigned int max_actions,
                              double avg_actions, std::optional<unsigned int> seed = std::nullopt)
            : RandomBuilder(seed), _max_depth{max_depth}, _num_goals{num_goals}, _max_actions{max_actions},
              _avg_actions{avg_actions} {
        _possible_actions.resize(_max_actions);
        std::iota(_possible_actions.begin(), _possible_actions.end(), 0);
    }

    [[nodiscard]] TreeState get_initial() override { return TreeState{}; }

    [[nodiscard]] std::unordered_set<TreeState> get_goal() override {
        // Generate a bunch of goal states. Not all goal states have the same depth.
        std::unordered_set<TreeState> goals(_num_goals);
        while (goals.size() < _num_goals) {
            unsigned int goal_depth = _depth_bino_dist(_prng_engine);
            TreeState goal{get_rand_actions(goal_depth, true)};
            goals.insert(std::move(goal));
        }
        return goals;
    }

    /// Builds an Adjacency List representation of a tree.
    [[nodiscard]] std::unique_ptr<BaseTransitionModel<TreeState>> get_transition_model() override {
        // Create an empty tree
        BasicTree::tree_t tree;
        auto max_size = static_cast<std::size_t>(pow(_max_actions, _max_depth));
        tree.reserve(max_size);

        // Fill the graph using Breadth-First-Search
        std::queue<TreeState> frontier({TreeState{}});
        tree.insert({TreeState{}, std::unordered_set<uint32_t>()});

        while (!frontier.empty()) {
            TreeState state{frontier.front()};
            frontier.pop();
            if (state.depth() == _max_depth) break; // Stop once we reach max_depth
            unsigned int n{_arity_bino_dist(_prng_engine)};
            for (const auto action: get_rand_actions(n, false)) {
                TreeState child{state, action};
                tree.insert({child, std::unordered_set<uint32_t>()});
                frontier.push(child);
                tree[state].insert(action);
            }
        }

        return std::make_unique<BasicTree>(std::move(tree));
    }

private:
    const unsigned int _max_depth;
    const unsigned int _num_goals;
    const unsigned int _max_actions;
    double _avg_actions;
    std::vector<uint32_t> _possible_actions;
    std::uniform_int_distribution<uint32_t> _udist{0, _max_actions - 1};
    std::binomial_distribution<unsigned int> _depth_bino_dist{_max_depth, 0.9};
    std::binomial_distribution<unsigned int> _arity_bino_dist{_max_actions, _avg_actions / _max_actions};

    [[nodiscard]] std::vector<uint32_t> get_rand_actions(unsigned int n, bool with_repetition) {
        std::vector<uint32_t> actions(n);
        if (with_repetition) {
            std::generate(actions.begin(), actions.end(), [this](){return _udist(_prng_engine); });
        } else {
            if (n > _max_actions) throw std::invalid_argument("n cannot be larger than the maximum number of actions possible for any given state.");
            std::ranges::shuffle(_possible_actions, _prng_engine);
            actions = {_possible_actions.cbegin(), _possible_actions.cbegin() + n};
        }
        return actions;
    }
};

#endif //PARALLEL_BFS_BASIC_TREE_BUILDER_H
