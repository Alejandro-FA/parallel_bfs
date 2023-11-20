#include <iostream>
#include "utils.h"
#include "problem/problem.h"
#include "problem_factory/random_graph_factory.h"
#include "bfs/sync_bfs.h"
#include "bfs/parallel_bfs.h"

int main() {
    // Choose which type of problem to create
    RandomGraphFactory factory{1'000'000, 4, 34};

    // Create random problem
    std::cout << "[INFO] Creating random problem..." << std::endl;
    auto start = std::chrono::high_resolution_clock::now();
    std::unique_ptr<Problem> problem{factory.make_problem()};
    auto stop = std::chrono::high_resolution_clock::now();
    std::cout << "[INFO] RandomGraph created. " << seconds_elapsed(start, stop) << "\n";
    std::cout << "\n" << *problem << "\n";

    // Solve created problem with method 1
    SyncBFS sync_bfs;
    measure(*problem, sync_bfs);

    // Solve created problem with method 2
    ParallelBFS par_bfs;
    measure(*problem, par_bfs);

    return 0;
}
