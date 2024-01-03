#include <iostream>
#include "utils.h"
#include "problem_factory/factories/random_graph_factory.h"
#include "problem_factory/factories/random_tree_factory.h"
#include "problem_factory/factories/procedural_tree_factory.h"
#include "bfs/sync_bfs.h"
#include "bfs/parallel_bfs.h"

int main() {
    // Choose which type of problem to create
    // RandomGraphFactory factory{1'000'000, 4, 34};
    // RandomTreeFactory factory{8, 6, 4.0, 3, 60};
    ProceduralTreeFactory factory{9, 7, 5.0, 5, 73};

    // Create random problem
    std::cout << "[INFO] Creating random problem..." << std::endl;
    auto start = std::chrono::high_resolution_clock::now();
    auto problem{factory.make_problem()};
    auto stop = std::chrono::high_resolution_clock::now();
    std::cout << "[INFO] Problem created. " << seconds_elapsed(start, stop) << "\n";
    std::cout << "\n" << *problem << "\n";

    // Solve created problem with method 1
    SyncBFS<SearchType::tree_like> sync_bfs;
    measure(*problem, sync_bfs, "Synchronous BFS");

    // Solve created problem with method 2
    ParallelBFS par_bfs;
    measure(*problem, par_bfs, "ParallelBFS");

    return 0;
}
