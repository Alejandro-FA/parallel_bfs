//
// Created by Alejandro Fernández on 29/11/2023.
//

#ifndef PARALLEL_BFS_PROJECT_BASIC_TREE_GENERATOR_H
#define PARALLEL_BFS_PROJECT_BASIC_TREE_GENERATOR_H

#include <queue>
#include <algorithm>
#include "tree_state.h"
#include "basic_tree.h"


namespace parallel_bfs {
    template<detail::UnsignedInteger T>
    class BasicTreeGenerator : public RandomFactory<TreeState<T>, BasicTree<T>> {
    public:
        /// Creates a random tree with max_depth and a maximum branching factor of max_actions. The average branching
        /// factor is specified by avg_actions, which should be a value between 0 and max_actions
        explicit BasicTreeGenerator(unsigned int max_depth, unsigned int num_goals, T max_actions, double avg_actions)
                : _max_depth{max_depth}, _num_goals{num_goals}, _max_actions{max_actions}, _avg_actions{avg_actions},
                  _possible_actions{get_iota_vector(max_actions)} {
            if (avg_actions > max_actions)
                throw std::invalid_argument(
                        "The average number of actions possible at each state must be lower or equal to the maximum number of actions possible.");
            if (num_goals > static_cast<unsigned int>(pow(max_actions, max_depth)))
                throw std::invalid_argument(
                        "The number of goals must be lower or equal to the maximum number of different states in the tree.");
        }

    protected:
        [[nodiscard]] TreeState<T> get_initial() override { return TreeState<T>{}; }

        [[nodiscard]] std::unordered_set<TreeState<T>> get_goal_states() override {
            // Generate a bunch of goal states. Not all goal states have the same depth.
            std::unordered_set<TreeState<T>> goals(_num_goals);
            while (goals.size() < _num_goals) {
                unsigned int goal_depth = this->get_random_value(_depth_binomial);
                TreeState goal{this->get_random_values(_udist, goal_depth)};
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
                T n{this->get_random_value(_branch_factor_binomial)};
                for (const auto action: this->get_random_sample(_possible_actions, n)) {
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
        const double _avg_actions;
        const std::vector<T> _possible_actions;
        std::uniform_int_distribution<T> _udist{0, _max_actions - 1};
        std::binomial_distribution<unsigned int> _depth_binomial{_max_depth, 0.9};
        std::binomial_distribution<T> _branch_factor_binomial{_max_actions, _avg_actions / _max_actions};

        [[nodiscard]] static std::vector<T> get_iota_vector(T n) {
            std::vector<T> output(n);
            std::iota(output.begin(), output.end(), 0);
            return output;
        }
    };
}


#endif //PARALLEL_BFS_PROJECT_BASIC_TREE_GENERATOR_H
