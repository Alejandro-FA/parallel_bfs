//
// Created by alejandro on 01/02/24.
//

#ifndef PARALLEL_BFS_PROJECT_SOLVE_H
#define PARALLEL_BFS_PROJECT_SOLVE_H

#include <filesystem>
#include <iostream>
#include <optional>
#include <vector>
#include <ranges>
#include <parallel_bfs/problem_utils.h>
#include <parallel_bfs/problems.h>
#include "utils.h"
#include "solver.h"


std::vector<std::filesystem::path> get_problem_files(const std::filesystem::path &dir, const std::string &extension, std::optional<unsigned int> max_files = std::nullopt) {
    std::vector<std::filesystem::path> result;

    if(std::filesystem::exists(dir) && std::filesystem::is_directory(dir)) {
        for (const auto& entry : std::filesystem::directory_iterator(dir)) {
            if(!entry.is_regular_file()) continue;
            const auto& file_path = entry.path();
            if(file_path.extension() == extension && file_path.filename().string().starts_with("problem")){
                result.push_back(file_path);
            }
            if (max_files.has_value() && result.size() >= max_files.value()) break;
        }
    }

    return result;
}


std::filesystem::path get_log_path(const std::filesystem::path &input_dir) {
    // Get current time
    auto now = std::chrono::system_clock::now();
    std::time_t current_time = std::chrono::system_clock::to_time_t(now);
    std::tm* time_info = std::localtime(&current_time);

    // Convert time to compact string
    std::stringstream file_name;
    file_name << "results_";
    file_name << std::put_time(time_info, "%Y-%m-%d-%H:%M:%S"); // Compact datetime format
    file_name << ".log";

    // Append file_name to input_dir
    return std::filesystem::weakly_canonical(input_dir / file_name.str());
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
    using StateType = parallel_bfs::TreeState<std::uint32_t>; // FIXME: Don't hardcode types
    using TransitionModelType = parallel_bfs::BasicTree<std::uint32_t>; // FIXME: Don't hardcode types

    // Create solver and add algorithms
    Solver<StateType , TransitionModelType> solver;
    solver.add_algorithm(parallel_bfs::sync_bfs<StateType, TransitionModelType, parallel_bfs::SearchType::tree_like>, "SyncBFS");
    solver.add_algorithm(parallel_bfs::parallel_bfs_tasks<StateType, TransitionModelType>, "ParallelBFSTasks");
    solver.add_algorithm(parallel_bfs::parallel_bfs_async<StateType, TransitionModelType>, "ParallelBFSAsync");

    // Create reader
    const parallel_bfs::YAMLReader<StateType, TransitionModelType> reader;
    const auto problem_files = get_problem_files(input_dir, reader.file_extension, num_problems);

    // Solve all problems with all algorithms
    std::cout << "\n[INFO] Solving problems from " << std::filesystem::canonical(input_dir) << "...\n";
    auto bar = SimpleProgressBar(problem_files.size() * 3, true);

    for (const auto &file_path : problem_files) {
        const auto file_name = file_path.filename().string();

        // Read problem
        bar.set_status("Reading " + file_name);
        const auto problem = reader.read(file_path);
        bar.tick();

        // Warm cache
        bar.set_status("Warming up cache");
        solver.warm_cache(problem);
        bar.tick();

        bar.set_status("Solving " + file_name);
        solver.solve(problem, file_name);
        bar.tick();
    }

    // Log results
    auto log_path = get_log_path(input_dir);
    std::ofstream log_stream{log_path};
    log_stream << solver.results() << "\nSUMMARY:\n" << solver.statistics_summary<Average, Median>();
    log_stream.close();
    std::cout << "[INFO] Results logged in " << log_path << ".\n";
}

#endif //PARALLEL_BFS_PROJECT_SOLVE_H
