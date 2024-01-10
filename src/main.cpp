#include <iostream>
#include <parallel_bfs/search.h>
#include "utils.h"
#include "example_problems/basic_graph/builder_bg.h"
#include "example_problems/basic_tree/builder_bt.h"

int main() {
    // Choose which type of problem to create
    // BasicGraphBuilder factory{1'000'000, 4, 34};
    BasicTreeBuilder factory{9, 5, 7, 5.0, 87};

    // Create random problem
    std::cout << "[INFO] Creating random problem..." << std::endl;
    auto start = std::chrono::high_resolution_clock::now();
    auto problem{factory.make_problem()};
    auto stop = std::chrono::high_resolution_clock::now();
    std::cout << "[INFO] Problem created. " << seconds_elapsed(start, stop) << "\n";
    std::cout << "\n" << *problem << "\n";

    // Solve created problem with method 1
    parallel_bfs::SyncBFS<parallel_bfs::SearchType::tree_like> sync_bfs;
    measure(*problem, sync_bfs, "Synchronous BFS");

    // Solve created problem with method 2
    parallel_bfs::ParallelBFS par_bfs;
    measure(*problem, par_bfs, "ParallelBFS");

    return 0;
}
