//
// Created by Alejandro Fernández on 04/11/2023.
//

#ifndef PARALLEL_BFS_UTILS_H
#define PARALLEL_BFS_UTILS_H

#include <chrono>
#include <stack>
#include <iomanip>
#include <sstream>
#include <unordered_set>
#include <unordered_map>
#include "node.h"
#include "bfs/bfs.h"

void print_solution(const Node *node) {
    // Check if there is a solution first
    if (!node) {
        std::cout << "No solution found!" << std::endl;
        return;
    }

    // If there is a solution, recreate the solution path
    std::stack<std::shared_ptr<State>> solution;
    while (node) {
        solution.push(node->state());
        node = node->parent().get();
    }

    // Print the solution
    std::cout << "Solution: " << *solution.top();
    solution.pop();
    while (!solution.empty()) {
        std::cout << " -> " << *solution.top();
        solution.pop();
    }
    std::cout << std::endl;
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

void measure(const Problem &problem, const BFS &bfs) {
    std::cout << "\n[INFO] Searching solution with " << bfs.get_name() << "..." << std::endl;
    auto start = std::chrono::high_resolution_clock::now();
    auto node = bfs(problem);
    auto stop = std::chrono::high_resolution_clock::now();
    std::cout << "[INFO] " << bfs.get_name() << " finished. " << ms_elapsed(start, stop) << "\n";
    print_solution(node.get());
}

/// Declaration of the concept "Hashable", which is satisfied by any type 'T'
/// such that for values 'a' of type 'T', the expression std::hash<T>{}(a)
/// compiles and its result is convertible to std::size_t
/// Copied from https://en.cppreference.com/w/cpp/language/constraints
template<typename T>
concept Hashable = requires(T a) {
    { std::hash<T>{}(a) } -> std::convertible_to<std::size_t>;
};

/// When hashing a shared_ptr with SharedPtrHash, the hash does not depend on the address
/// of the pointer, but on the underlying instance. Only works for pointers of objects
/// that are hashable.
struct SharedPtrHash {
    template<Hashable T>
    std::size_t operator()(const std::shared_ptr<T> &ptr) const {
        if (ptr == nullptr) return 0; // ToDo: Search if this should be handled differently
        return std::hash<T>()(*ptr); // Use the hash of the underlying object
    }
};

/// When comparing 2 shared_ptr using SharedPtrEqual, the result does not depend on the
/// address of the pointer, but on the underlying instance. Only works for pointers of
/// objects that are comparable.
struct SharedPtrEqual {
    template<Hashable T>
    std::size_t operator()(const std::shared_ptr<T> &lhs, const std::shared_ptr<T> &rhs) const {
        if (lhs == nullptr && rhs == nullptr) return true;
        if (lhs == nullptr || rhs == nullptr) return false;
        return *lhs == *rhs;
    }
};

template<Hashable T>
using unordered_set_ptr = std::unordered_set<std::shared_ptr<T>, SharedPtrHash, SharedPtrEqual>;

template<Hashable K, typename V>
using unordered_map_ptr = std::unordered_map<std::shared_ptr<K>, V, SharedPtrHash, SharedPtrEqual>;

#endif //PARALLEL_BFS_UTILS_H
