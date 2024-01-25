//
// Created by Alejandro Fernández on 29/11/2023.
//

#ifndef PARALLEL_BFS_BASIC_TREE_GENERATOR_H
#define PARALLEL_BFS_BASIC_TREE_GENERATOR_H

#include <queue>
#include <algorithm>
#include <parallel_bfs/search.h>
#include "tree_state.h"
#include "basic_tree.h"

template<UnsignedInteger T>
class BasicTreeGenerator : public parallel_bfs::RandomFactory<TreeState<T>, BasicTree<T>> {
public:
    /// Creates a random tree with max_depth and a maximum branching factor of max_actions. The average branching
    /// factor is specified by avg_actions, which should be a value between 0 and max_actions
    explicit BasicTreeGenerator(unsigned int max_depth, unsigned int num_goals, T max_actions,
                                double avg_actions, std::optional<unsigned int> seed = std::nullopt)
            : parallel_bfs::RandomFactory<TreeState<T>, BasicTree<T>>(seed), _max_depth{max_depth},
              _num_goals{num_goals}, _max_actions{max_actions}, _avg_actions{avg_actions} {
        if (avg_actions > max_actions)
            throw std::invalid_argument("The average number of actions possible at each state must be lower or equal to the maximum number of actions possible.");
        if (num_goals > static_cast<unsigned int>(pow(max_actions, max_depth)))
            throw std::invalid_argument("The number of goals must be lower or equal to the maximum number of different states in the tree.");
        _possible_actions.resize(_max_actions);
        std::iota(_possible_actions.begin(), _possible_actions.end(), 0);
    }

protected:
    [[nodiscard]] TreeState<T> get_initial() override { return TreeState<T>{}; }

    [[nodiscard]] std::unordered_set<TreeState<T>> get_goal_states() override {
        // Generate a bunch of goal states. Not all goal states have the same depth.
        std::unordered_set<TreeState<T>> goals(_num_goals);
        while (goals.size() < _num_goals) {
            unsigned int goal_depth = _depth_bino_dist(this->_prng_engine);
            TreeState goal{get_rand_actions(goal_depth, true)};
            goals.insert(std::move(goal));
        }
        return goals;
    }

    /// Builds an Adjacency List representation of a tree.
    [[nodiscard]] BasicTree<T> get_transition_model() override {
        // Create an empty tree
        auto max_size = static_cast<std::size_t>(pow(_max_actions, _max_depth));
        BasicTree<T> tree(max_size);

        // Fill the graph using Breadth-First-Search
        std::queue<TreeState<T>> frontier({TreeState<T>{}});
        tree.insert(TreeState<T>{}, std::unordered_set<T>());

        while (!frontier.empty()) {
            TreeState<T> state{frontier.front()};
            frontier.pop();
            if (state.depth() == _max_depth) break; // Stop once we reach max_depth
            T n{_arity_bino_dist(this->_prng_engine)};
            for (const auto action: get_rand_actions(n, false)) {
                TreeState<T> child{state, action};
                tree.insert(child, std::unordered_set<T>());
                frontier.push(child);
                tree[state].insert(action);
            }
        }

        return BasicTree<T>{std::move(tree)};
    }

private:
    const unsigned int _max_depth;
    const unsigned int _num_goals;
    const unsigned int _max_actions;
    double _avg_actions;
    std::vector<T> _possible_actions;
    std::uniform_int_distribution<T> _udist{0, _max_actions - 1};
    std::binomial_distribution<unsigned int> _depth_bino_dist{_max_depth, 0.9};
    std::binomial_distribution<T> _arity_bino_dist{_max_actions, _avg_actions / _max_actions};


    [[nodiscard]] std::vector<T> get_rand_actions(unsigned int n, bool with_repetition) {
        std::vector<T> actions(n);
        if (with_repetition) {
            std::generate(actions.begin(), actions.end(), [this]() { return _udist(this->_prng_engine); });
        } else {
            if (n > _max_actions)
                throw std::invalid_argument("n cannot be larger than the maximum number of actions possible for any given state.");
            std::ranges::shuffle(_possible_actions, this->_prng_engine);
            actions = {_possible_actions.cbegin(), _possible_actions.cbegin() + n};
        }
        return actions;
    }
};


#endif //PARALLEL_BFS_BASIC_TREE_GENERATOR_H
