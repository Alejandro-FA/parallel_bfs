//
// Created by Alejandro Fernández on 29/11/2023.
//

#ifndef PARALLEL_BFS_PROJECT_BASIC_TREE_GENERATOR_H
#define PARALLEL_BFS_PROJECT_BASIC_TREE_GENERATOR_H

#include <queue>
#include <algorithm>
#include "tree_state.h"
#include "basic_tree.h"
#include "../problems_common.h"


namespace parallel_bfs::detail {
    enum class BranchingDistribution { Uniform, Binomial };
}


namespace parallel_bfs {
    template<detail::UnsignedInteger T>
    class BasicTreeGenerator : public RandomFactory<TreeState<T>, BasicTree<T>> {
    public:
        /**
         * @brief Creates a random tree generator with specified parameters.
         *
         * @param max_depth The maximum depth of the tree.
         * @param num_goals The number of goal states in the tree.
         * @param avg_bfactor The average branching factor of the tree.
         * @param max_bfactor The maximum branching factor of the tree.
         *
         * @throw std::invalid_argument If avg_bfactor is greater than max_bfactor or less than 0.
         * @throw std::invalid_argument If num_goals is greater than the maximum number of different states in the tree.
         */
        explicit BasicTreeGenerator(unsigned int max_depth, unsigned int goals_depth, unsigned int num_goals, double avg_bfactor, unsigned int max_bfactor)
                : _max_depth{max_depth}, _goals_depth{goals_depth}, _num_goals{num_goals}, _avg_bfactor{avg_bfactor},
                  _min_bfactor{0}, _max_bfactor{max_bfactor}, _bdist{detail::BranchingDistribution::Binomial} {
            if (avg_bfactor > max_bfactor || avg_bfactor < 0)
                throw std::invalid_argument(
                    "The average branching factor must be a value between 0 and max_bfactor.");
            if (goals_depth > max_depth)
                throw std::invalid_argument(
                    "The depth of the goal states must be lower or equal to the maximum depth of the tree.");
            if (num_goals > static_cast<unsigned int>(pow(max_bfactor, goals_depth)))
                throw std::invalid_argument(
                    "The number of goals must be lower or equal to the maximum number of different states in the tree.");
        }

        /**
          * @brief Constructs a BasicTreeGenerator object with the given parameters.
          *
          * @param max_depth The maximum depth of the tree.
          * @param num_goals The number of goal states in the tree.
          * @param min_bfactor The minimum branching factor of the tree.
          * @param max_bfactor The maximum branching factor of the tree.
          *
          * @throw std::invalid_argument If the minimum branching factor is greater than the maximum branching factor.
          * @throw std::invalid_argument If the number of goals is greater than the maximum number of different states in the tree.
          */
        explicit BasicTreeGenerator(unsigned int max_depth, unsigned int goals_depth, unsigned int num_goals, unsigned int min_bfactor, unsigned int max_bfactor)
                : _max_depth{max_depth}, _goals_depth{goals_depth}, _num_goals{num_goals}, _avg_bfactor{(min_bfactor + max_bfactor) / 2.0},
                  _min_bfactor{min_bfactor}, _max_bfactor{max_bfactor}, _bdist{detail::BranchingDistribution::Uniform} {
            if (min_bfactor > max_bfactor)
                throw std::invalid_argument(
                    "The minimum branching factor must be lower or equal to the maximum branching factor.");
            if (goals_depth > max_depth)
                throw std::invalid_argument(
                    "The depth of the goal states must be lower or equal to the maximum depth of the tree.");
            if (num_goals > static_cast<unsigned int>(pow(max_bfactor, goals_depth)))
                throw std::invalid_argument(
                    "The number of goals must be lower or equal to the maximum number of different states in the tree.");
        }

    protected:
        [[nodiscard]] TreeState<T> get_initial() override { return TreeState<T>{}; }

        /// Generate a bunch of goal states. Not all goal states have the same depth.
        [[nodiscard]] std::unordered_set<TreeState<T>> get_goal_states() override {
            if (_num_goals > 0.2 * static_cast<unsigned int>(pow(_max_bfactor, _goals_depth)))
                throw std::invalid_argument(
                        "The number of goals is too high. It must be lower than 20% of the maximum number of possible goals.");
            std::unordered_set<TreeState<T>> goals(_num_goals);
            while (goals.size() < _num_goals) goals.insert(get_random_goal()); // FIXME: Not efficient if num_goals is close to the maximum number of states
            return goals;
        }

        /// Builds an Adjacency List representation of a tree.
        [[nodiscard]] BasicTree<T> get_transition_model() override {
            // Create an empty tree
            auto expected_size = static_cast<std::size_t>(pow(_avg_bfactor, _max_depth) * 1.1); // 10% more than needed to avoid rehashing
            BasicTree<T> tree(expected_size);

            // Fill the graph using Breadth-First-Search
            std::queue<TreeState<T>> frontier({TreeState<T>{}});
            tree.insert(TreeState<T>{}, std::unordered_set<T>());

            while (!frontier.empty()) {
                TreeState<T> state{frontier.front()};
                frontier.pop();
                if (state.depth() == _max_depth) break; // Stop once we reach max_depth
                for (const auto action: get_random_actions()) {
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
        const unsigned int _goals_depth;
        const unsigned int _num_goals;
        const double _avg_bfactor;
        const unsigned int _min_bfactor;
        const unsigned int _max_bfactor;
        const detail::BranchingDistribution _bdist;
        const std::vector<T> _possible_actions {get_possible_actions(_max_bfactor)};

        [[nodiscard]] TreeState<T> get_random_goal() {
            // // Randomly select a depth for the goal state
            // std::binomial_distribution<unsigned int> _depth_binomial{_max_depth, 0.9};
            // unsigned int depth = this->get_random_value(_depth_binomial);
            unsigned int depth = _goals_depth;

            // Randomly select a path to the goal state
            std::uniform_int_distribution<T> udist{0, static_cast<T>(_max_bfactor - 1)};
            auto goal_path = this->get_random_values(udist, depth);
            return TreeState{goal_path};
        }

        [[nodiscard]] std::vector<T> get_random_actions() {
            if (_bdist == detail::BranchingDistribution::Uniform) {
                std::uniform_int_distribution<unsigned int> dist{_min_bfactor, _max_bfactor};
                unsigned int branch_factor = this->get_random_value(dist);
                return this->get_random_sample(_possible_actions, branch_factor);
            } else if (_bdist == detail::BranchingDistribution::Binomial) {
                std::binomial_distribution<unsigned int> dist{_max_bfactor, _avg_bfactor / _max_bfactor};
                unsigned int branch_factor = this->get_random_value(dist);
                return this->get_random_sample(_possible_actions, branch_factor);
            } else {
                throw std::invalid_argument("Invalid branching distribution selected.");
            }
        }

        [[nodiscard]] static std::vector<T> get_possible_actions(T n) {
            std::vector<T> output(n);
            std::iota(output.begin(), output.end(), 0);
            return output;
        }
    };
}


#endif //PARALLEL_BFS_PROJECT_BASIC_TREE_GENERATOR_H
