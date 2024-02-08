//
// Created by alejandro on 08/02/24.
//

#ifndef PARALLEL_BFS_PROJECT_SOLVER_H
#define PARALLEL_BFS_PROJECT_SOLVER_H

#include <stack>
#include <iostream>
#include <vector>
#include <unordered_map>
#include <random>
#include <functional>
#include <ranges>
#include <parallel_bfs/search.h>
#include <parallel_bfs/problem_utils.h>
#include "utils.h"
#include "statistics.h"


template<parallel_bfs::Searchable State>
std::string solution_path(const parallel_bfs::Node<State> *node) {
    std::stringstream stream;
    if (!node) return "No solution found!";

    std::stack<State> solution;
    while (node) {
        solution.push(node->state());
        node = node->parent().get();
    }

    stream << solution.top();
    solution.pop();
    while (!solution.empty()) {
        stream << " -> " << solution.top();
        solution.pop();
    }

    return stream.str();
}


template<typename F, typename State, typename TM>
concept BfsCallable = requires(F&& f, const parallel_bfs::Problem<State, TM> &problem) {
    requires parallel_bfs::Searchable<State>;
    requires std::derived_from<TM, parallel_bfs::BaseTransitionModel<State>>;
    requires std::invocable<F, const parallel_bfs::Problem<State, TM> &>;
    { std::invoke<F, const parallel_bfs::Problem<State, TM> &>(std::forward<F>(f), problem) } -> std::same_as<std::shared_ptr<parallel_bfs::Node<State>>>;
};


template<parallel_bfs::Searchable State, std::derived_from<parallel_bfs::BaseTransitionModel<State>> TM>
struct BfsAlgorithm {
    std::function<std::shared_ptr<parallel_bfs::Node<State>>(const parallel_bfs::Problem<State, TM>&)> algorithm;
    std::string name;
};


template<parallel_bfs::Searchable State>
struct Measurement {
    std::string problem_name;
    std::string algorithm_name;
    ExecutionTime time;
    std::shared_ptr<parallel_bfs::Node<State>> solution;
};


template<parallel_bfs::Searchable State, std::derived_from<parallel_bfs::BaseTransitionModel<State>> TM>
class Solver {
public:
    void add_algorithm(BfsCallable<State,TM> auto &&f, std::string&& name) {
        _bfs_functions.emplace_back(std::forward<decltype(f)>(f), std::move(name));
    }

    void warm_cache(const parallel_bfs::Problem<State, TM> &problem) {
        std::ranges::shuffle(_bfs_functions, _random_engine); // Shuffle to reduce the effect of caching
        std::invoke(_bfs_functions[0].algorithm, problem); // Cache warming
    }

    void solve(const parallel_bfs::Problem<State, TM> &problem, std::string problem_name) {
        for (const auto & [algo, algo_name] : _bfs_functions) {
            auto [solution, time] = invoke_and_time(algo, problem);
            _results.emplace_back(problem_name, algo_name, time, solution);
        }
    }

    [[nodiscard]] std::string results() const {
        std::stringstream stream;
        auto grouped = group_by_problem();
        for (const auto &[problem_name, measurements] : grouped) {
            stream << problem_name << "\n";
            for (const auto &m : measurements) {
                stream << m.algorithm_name << ": [" << m.time.as_milliseconds() << " ms], "
                       << solution_path(m.solution.get()) << "\n";
            }
            stream << "\n";
        }
        return stream.str();
    }

    template<std::derived_from<Statistic>... Stats> requires (sizeof...(Stats) > 0)
    [[nodiscard]] std::string statistics_summary() const {
        std::stringstream stream;
        auto grouped = group_by_algorithm();
        for (const auto &[algo_name, measurements] : grouped) {
            stream << algo_name << '\n';
            std::vector<double> times(measurements.size());
            std::ranges::transform(measurements, times.begin(), [](const auto &m) { return m.time.as_milliseconds(); });
            ((stream << "\t" << Stats{}.name() << ": " << Stats{}.compute(times) << " ms\n"), ...);
        }
        return stream.str();
    }

private:
    [[nodiscard]] std::unordered_map<std::string, std::vector<Measurement<State>>> group_by_problem() const {
        std::unordered_map<std::string, std::vector<Measurement<State>>> grouped;
        for (const auto &result : _results) grouped[result.problem_name].push_back(result);
        return grouped;
    }

    [[nodiscard]] std::unordered_map<std::string, std::vector<Measurement<State>>> group_by_algorithm() const {
        std::unordered_map<std::string, std::vector<Measurement<State>>> grouped;
        for (const auto &result : _results) grouped[result.algorithm_name].push_back(result);
        return grouped;
    }

    std::vector<BfsAlgorithm<State, TM>> _bfs_functions;
    std::vector<Measurement<State>> _results;
    std::default_random_engine _random_engine{std::random_device{}()};
};



#endif //PARALLEL_BFS_PROJECT_SOLVER_H
