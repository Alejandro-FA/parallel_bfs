//
// Created by alejandro on 30/01/24.
//

#ifndef PARALLEL_BFS_PROJECT_UTILS_H
#define PARALLEL_BFS_PROJECT_UTILS_H

#include <filesystem>
#include <stack>
#include <parallel_bfs/search.h>


std::filesystem::path get_build_dir_parent() {
    std::filesystem::path current_path = std::filesystem::current_path();
    if (current_path.filename().string().starts_with("cmake-build")) {
        current_path = current_path.parent_path();
    }
    return current_path;
}


template<parallel_bfs::Searchable State>
void log_solution(const parallel_bfs::Node<State> *node, std::ostream& log_stream = std::cout) {
    // Check if there is a solution first
    if (!node) {
        std::cout << "No solution found!" << std::endl;
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
    log_stream << std::endl;
}


std::string seconds_elapsed(const std::chrono::time_point<std::chrono::high_resolution_clock> &start_time,
                            const std::chrono::time_point<std::chrono::high_resolution_clock> &end_time) {
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
    std::stringstream sstr;
    sstr << std::fixed << std::setprecision(3) << (double) duration / 1000.0;
    return "[" + sstr.str() + "s]";
}


std::string ms_elapsed(const std::chrono::time_point<std::chrono::high_resolution_clock> &start_time,
                       const std::chrono::time_point<std::chrono::high_resolution_clock> &end_time) {
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count();
    std::stringstream sstr;
    sstr << std::fixed << std::setprecision(2) << (double) duration / 1000.0;
    return "[" + sstr.str() + "ms]";
}


template<std::invocable F>
std::invoke_result_t<F> run_and_measure(F &&f, const std::string &message, std::ostream& log_stream = std::cout) {
    log_stream << "\n[INFO] " << message << "..." << std::endl;
    auto start = std::chrono::high_resolution_clock::now();

    if constexpr (std::is_same_v<std::invoke_result_t<F>, void>) {
        f();
        auto stop = std::chrono::high_resolution_clock::now();
        log_stream << "[INFO] " << message << " finished. " << ms_elapsed(start, stop) << std::endl;
        return;
    } else {
        auto output = f();
        auto stop = std::chrono::high_resolution_clock::now();
        log_stream << "[INFO] " << message << " finished. " << ms_elapsed(start, stop) << std::endl;
        return output;
    }
}

#endif //PARALLEL_BFS_PROJECT_UTILS_H
