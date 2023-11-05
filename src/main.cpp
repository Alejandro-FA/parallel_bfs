#include <iostream>
#include <chrono>
#include "problem.h"
#include "bfs.h"
#include "utils.h"

int main() {
    // Create random problem
    std::cout << "[INFO] Creating random problem..." << std::endl;
    auto start = std::chrono::high_resolution_clock::now();
    // Problem problem{1000000, 4, 242342};
    Problem problem{1000000, 4};
    auto stop = std::chrono::high_resolution_clock::now();
    std::cout << "[INFO] Problem created. " << seconds_elapsed(start, stop) << "\n";
    problem.print(false);

    // Solve created problem
    std::cout << "[INFO] Searching solution..." << std::endl;
    start = std::chrono::high_resolution_clock::now();
    auto node = breadth_first_search(problem);
    stop = std::chrono::high_resolution_clock::now();
    std::cout << "[INFO] Search finished. " << ms_elapsed(start, stop) << "\n";
    print_solution(node.get());

    return 0;
}
