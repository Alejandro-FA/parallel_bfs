//
// Created by Alejandro Fernández on 04/11/2023.
//

#ifndef PARALLEL_BFS_UTILS_H
#define PARALLEL_BFS_UTILS_H

#include <chrono>
#include <stack>
#include <iomanip>
#include <sstream>
#include "node.h"
#include "state.h"
#include "bfs/bfs.h"

void print_solution(const Node* node) {
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
    std::cout << "Solution: " << solution.top();
    solution.pop();
    while (!solution.empty()) {
        std::cout << " -> " << solution.top();
        solution.pop();
    }
    std::cout << std::endl;
}

std::string seconds_elapsed(const std::chrono::time_point<std::chrono::high_resolution_clock> &start_time,
                            const std::chrono::time_point<std::chrono::high_resolution_clock> &end_time){
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time-start_time).count();
    std::stringstream sstr;
    sstr << std::fixed << std::setprecision(3) << (double) duration / 1000.0;
    return "["+ sstr.str()+"s]";
}

std::string ms_elapsed(const std::chrono::time_point<std::chrono::high_resolution_clock> &start_time,
                            const std::chrono::time_point<std::chrono::high_resolution_clock> &end_time){
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time-start_time).count();
    std::stringstream sstr;
    sstr << std::fixed << std::setprecision(2) << (double) duration / 1000.0;
    return "["+ sstr.str()+"ms]";
}

void measure(const Problem &problem, const BFS &bfs) {
    std::cout << "\n[INFO] Searching solution with " << bfs.get_name() << "..."  << std::endl;
    auto start = std::chrono::high_resolution_clock::now();
    auto node = bfs(problem);
    auto stop = std::chrono::high_resolution_clock::now();
    std::cout << "[INFO] " << bfs.get_name() << " finished. " << ms_elapsed(start, stop) << "\n";
    print_solution(node.get());
}

#endif //PARALLEL_BFS_UTILS_H
