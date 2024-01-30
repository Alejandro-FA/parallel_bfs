//
// Created by alejandro on 04/01/24.
//

#ifndef PARALLEL_BFS_TRANSITION_MODEL_H
#define PARALLEL_BFS_TRANSITION_MODEL_H

#include <vector>
#include "state.h"


namespace parallel_bfs {
    template<Searchable State>
    class BaseTransitionModel {
    public:
        virtual ~BaseTransitionModel() = default;

        /// Given a state, it returns all possible next states and the cost of going to those states
        [[nodiscard]] virtual std::vector<std::pair<State, int>> next_states(const State &state) const = 0;
    };


    template<Searchable State, std::regular Action>
    class TransitionModel : public BaseTransitionModel<State> {
    public:
        // TODO: perhaps change this to a coroutine when std::generator (C++23) is implemented in gcc/clang
        [[nodiscard]] std::vector<std::pair<State, int>> next_states(const State &state) const final {
            std::vector<std::pair<State, int>> next_states;
            for (const Action &action: actions(state)) {
                State new_state = result(state, action);
                int cost = action_cost(state, action, new_state);
                next_states.emplace_back(new_state, cost);
            }
            return next_states;
        }

        [[nodiscard]] virtual std::vector<Action> actions(const State &state) const = 0;

        [[nodiscard]] virtual int action_cost(const State &current, const Action &action, const State &next) const = 0;

        [[nodiscard]] virtual State result(const State &state, const Action &action) const = 0;
    };
}

#endif //PARALLEL_BFS_TRANSITION_MODEL_H
