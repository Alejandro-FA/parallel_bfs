//
// Created by alejandro on 01/02/24.
//

#ifndef PARALLEL_BFS_PROJECT_SOLVE_H
#define PARALLEL_BFS_PROJECT_SOLVE_H

#include <filesystem>
#include <iostream>
#include <format>
#include <optional>
#include <stack>
#include <parallel_bfs/search.h>
#include <parallel_bfs/problem_utils.h>
#include <parallel_bfs/problems.h>
#include "utils.h"


template<parallel_bfs::Searchable State>
void log_solution(const parallel_bfs::Node<State> *node, std::ostream& log_stream = std::cout) {
    // Check if there is a solution first
    if (!node) {
        log_stream << "No solution found!\n";
        return;
    }

    // If there is a solution, recreate the solution path
    std::stack<State> solution;
    while (node) {
        solution.push(node->state());
        node = node->parent().get();
    }

    // Print the solution
    log_stream << "Solution: " << solution.top();
    solution.pop();
    while (!solution.empty()) {
        log_stream << " -> " << solution.top();
        solution.pop();
    }
    log_stream << "\n";
}


/**
* Function: solve
* ----------------
* Reads search problems from the directory @input_dir and solves them.
*
* @param input_dir: a path to the directory from where the problems will be read.
* @param num_problems: optional parameter, the number of problems to solve. If not provided, all problems in the directory will be solved.
*
* NOTE: This function does NOT validate if @input_dir is a valid directory.
*/
void solve(const std::filesystem::path &input_dir, std::optional<unsigned int> num_problems = std::nullopt) {
    using namespace parallel_bfs;
    using StateType = TreeState<std::uint32_t>; // FIXME: Don't hardcode types
    using TransitionModelType = BasicTree<std::uint32_t>; // FIXME: Don't hardcode types
    using BfsFunction = std::function<std::shared_ptr<Node<StateType>>(const Problem<StateType, TransitionModelType> &)>;

    std::vector<std::pair<BfsFunction, std::string>> bfs_functions {
        std::make_pair(sync_bfs<StateType, TransitionModelType, SearchType::tree_like>, "SyncBFS"),
        std::make_pair(parallel_bfs_tasks<StateType, TransitionModelType>, "ParallelBFSTasks"),
        std::make_pair(parallel_bfs_async<StateType, TransitionModelType>, "ParallelBFSAsync"),
    };

    const YAMLReader<StateType, TransitionModelType> reader;
    std::filesystem::path log_path = std::filesystem::weakly_canonical(input_dir / "results_log.txt");
    std::ofstream log_stream{log_path};

    unsigned int count = 0;
    std::default_random_engine random_engine{std::random_device{}()};

    for (const auto & entry : std::filesystem::directory_iterator(input_dir)) {
        const auto file_path = std::filesystem::canonical(entry.path());
        if (file_path.extension() != reader.file_extension || !file_path.filename().string().starts_with("problem")) continue;
        if (count > 0) log_stream << "\n";

        // Read problem
        std::cout << "\n[INFO] Reading problem from " << file_path << "..." << std::flush;
        auto [problem, reading_time] = invoke_and_time([&] { return reader.read(file_path); });
        std::cout << " [" << reading_time.as_seconds() << " s]" << std::endl;

        // Shuffle the algorithms to reduce the effect of caching
        std::ranges::shuffle(bfs_functions, random_engine);

        // Cache warming
        std::cout << "[INFO] Warming up cache..." << std::flush;
        auto [_, warming_time] = invoke_and_time(bfs_functions[0].first, problem);
        std::cout << " [" << warming_time.as_milliseconds() << " ms]" << std::endl;

        // Solve problems with all the methods
        std::cout << "[INFO] Solving " << file_path.filename() << " with multiple algorithms..." << std::flush;
        log_stream << "Problem: " << file_path.filename() << "\n";
        double total_time = 0;
        for (const auto & [algo, algo_name] : bfs_functions) {
            auto [solution, time] = invoke_and_time(algo, problem);
            log_stream << algo_name << ": [" << time.as_milliseconds() << " ms], ";
            log_solution(solution.get(), log_stream);
            total_time += time.as_milliseconds();
        }
        std::cout << " [" << total_time << " ms]" << std::endl;

        count++;
        if (num_problems.has_value() && count >= num_problems.value()) break; // Don't read more than num_problems
    }

    std::cout << "\n[INFO] Results logged in " << log_path << ".\n";
}

#endif //PARALLEL_BFS_PROJECT_SOLVE_H
