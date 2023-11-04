#include <iostream>
#include <chrono>
#include "problem.h"
#include "bfs.h"
#include "utils.h"

using namespace std::chrono;

int main() {
    // Create random problems
    auto start = high_resolution_clock::now();
    // Problem p{10000, 34535234};
    Problem p{10};
    auto stop = high_resolution_clock::now();
    std::cout << "[INFO] Problem created. " << seconds_elapsed(start, stop) << std::endl;
    std::cout << p << std::endl;

    // Solve random problem
    start = high_resolution_clock::now();
    auto node = breadth_first_search(p);
    stop = high_resolution_clock::now();
    std::cout << "[INFO] Search finished. " << seconds_elapsed(start, stop) << std::endl;
    print_solution(node.get());

    return 0;
}
