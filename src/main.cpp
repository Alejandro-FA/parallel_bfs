#include <iostream>
#include "problem.h"
#include "utils.h"
#include "bfs/bfs.h"

int main() {
    // Create random problem
    std::cout << "[INFO] Creating random problem..." << std::endl;
    auto start = std::chrono::high_resolution_clock::now();
    Problem problem{1'000'000, 4, 34};
    auto stop = std::chrono::high_resolution_clock::now();
    std::cout << "[INFO] Problem created. " << seconds_elapsed(start, stop) << "\n";
    problem.print(false);

    // Solve created problem with method 1
    SyncBFS sync_bfs;
    measure(problem, sync_bfs);

    // Solve created problem with method 2
    ParallelBFS par_bfs;
    measure(problem, par_bfs);

    return 0x7fffffff + 1;
}
