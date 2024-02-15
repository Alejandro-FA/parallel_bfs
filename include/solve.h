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
#include <parallel_bfs/search.h>
#include "utils.h"
#include "solver.h"


/**
 * @brief Get a list of problem files in a directory that match the given extension.
 *
 * This function searches for files in the specified directory that have the given extension and start with "problem".
 * The function optionally allows specifying a maximum number of files to retrieve.
 *
 * @param dir The directory to search for problem files.
 * @param extension The file extension to match.
 * @param max_files Optional maximum number of files to retrieve. Defaults to retrieving all matching files.
 * @return A vector of paths to the problem files.
 * @note If the directory does not exist or is not a directory, an empty vector will be returned.
 */
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


/**
 * @brief Get the path for a log file based on the current time and a given input directory.
 *
 * This function generates a log file name in the format "results_YYYY-MM-DD-HH:MM:SS.log"
 * based on the current system time, and appends it to the given input directory. The resulting
 * file path is returned.
 *
 * @param input_dir The input directory path where the log file will be placed.
 * @return The path to the log file.
 */
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
    return input_dir / file_name.str();
}


/**
 * @brief Logs the results of the Solver.
 *
 * This function logs the results of the Solver to a file in the given input directory.
 * It also prints a performance statistics summary to the console.
 *
 * @tparam State The searchable state type.
 * @tparam TM The transition model type.
 * @param input_dir The input directory path.
 * @param solver The Solver object.
 */
template<parallel_bfs::Searchable State, std::derived_from<parallel_bfs::BaseTransitionModel<State>> TM>
void log_results(const std::filesystem::path &input_dir, const Solver<State, TM> &solver) {
    auto log_path = get_log_path(input_dir);
    std::ofstream log_stream{log_path};
    const auto stats = solver.template statistics_summary<Average, Median, StandardDeviation>();
    log_stream << solver.results() << "\n[INFO] Results summary:\n" << stats;
    std::cout << "\n[INFO] Results summary:\n" << stats << "\n";
    std::cout << "[INFO] Detailed results logged in " << log_path << "." << std::endl;
}


/**
 * @brief Solve a set of problems using various algorithms.
 *
 * Given an input directory and an optional number of problems to solve, this function reads the problem files
 * from the directory, solves each problem using multiple algorithms, and logs the results.
 *
 * @param input_dir The directory containing the problem files.
 * @param num_problems Optional. The number of problems to solve. If not specified, all problems will be solved.
 * @note This function does NOT validate if @input_dir is a valid directory.
 */
void solve(const std::filesystem::path &input_dir, std::optional<unsigned int> num_problems, std::optional<std::chrono::microseconds> workload_delay) noexcept(false) {
    using StateType = parallel_bfs::TreeState<std::uint32_t>; // FIXME: Don't hardcode types
    using TransitionModelType = parallel_bfs::BasicTree<std::uint32_t>; // FIXME: Don't hardcode types

    // Create solver and add algorithms
    Solver<StateType , TransitionModelType> solver;
    solver.add_algorithm(parallel_bfs::sync_bfs<StateType, TransitionModelType, parallel_bfs::SearchType::tree_like>, "SyncBFS");
    solver.add_algorithm(parallel_bfs::tasks_bfs<StateType, TransitionModelType>, "TasksBFS");
    solver.add_algorithm(parallel_bfs::async_start_bfs<StateType, TransitionModelType>, "AsyncStartBFS");
    solver.add_algorithm(parallel_bfs::async_bfs<StateType, TransitionModelType>, "AsyncBFS"); // Very slow
    solver.add_algorithm(parallel_bfs::foreach_start_bfs<StateType, TransitionModelType>, "ForeachStartBFS");
    solver.add_algorithm(parallel_bfs::foreach_bfs<StateType, TransitionModelType>, "ForeachBFS"); // Very slow
    solver.add_algorithm(parallel_bfs::any_of_bfs<StateType, TransitionModelType>, "AnyOfBFS");
    solver.add_algorithm(parallel_bfs::multithread_bfs<StateType, TransitionModelType>, "MultithreadBFS");

    // Create reader
    const parallel_bfs::YAMLReader<StateType, TransitionModelType> reader;
    const auto problem_files = get_problem_files(input_dir, reader.file_extension, num_problems);
    if (problem_files.empty()) throw std::runtime_error{"No problem files found in \"" + input_dir.string() + '"'};

    // Solve all problems with all algorithms
    std::cout << "\n[INFO] Solving " << problem_files.size() << " problems from " << input_dir << "...\n";
    std::cout << "[INFO] CPU cores available: " << std::thread::hardware_concurrency() << std::endl;
    auto bar = SimpleProgressBar(problem_files.size() * 3, true);

    for (const auto &file_path : problem_files) {
        const auto file_name = file_path.filename().string();

        // Read problem
        bar.set_status("Reading " + file_name);
        auto problem = reader.read(file_path);
        if (workload_delay.has_value()) problem.set_workload_delay(workload_delay.value());
        bar.tick();

        // Warm cache
        bar.set_status("Warming up cache");
        solver.warm_cache(problem);
        bar.tick();

        bar.set_status("Solving " + file_name);
        solver.solve(problem, file_name);
        bar.tick();
    }

    log_results(input_dir, solver);
}

#endif //PARALLEL_BFS_PROJECT_SOLVE_H
