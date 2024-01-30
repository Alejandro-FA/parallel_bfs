#include <iostream>
#include <fstream>
#include <parallel_bfs/search.h>
#include <parallel_bfs/problem_utils.h>
#include "example_problems/basic_graph/basic_graph_generator.h"
#include "example_problems/basic_tree/basic_tree_generator.h"
#include "utils.h"


int main() {
    // Choose which type of problem to create
    // BasicGraphGenerator<std::uint32_t> generator{1'000'000, 4, 34};
    BasicTreeGenerator<std::uint32_t> generator{9, 5, 7, 5.0, 87};

    // Create random problem
    std::cout << "[INFO] Creating random problem..." << std::endl;
    auto start = std::chrono::high_resolution_clock::now();
    parallel_bfs::Problem problem{generator.make_problem()};
    auto stop = std::chrono::high_resolution_clock::now();
    std::cout << "[INFO] Problem created. " << parallel_bfs::seconds_elapsed(start, stop) << "\n";
    std::cout << "\n" << problem << "\n";

    // Solve created problem with method 1
    parallel_bfs::SyncBFS<parallel_bfs::SearchType::tree_like> sync_bfs;
    measure(problem, sync_bfs, "Synchronous BFS");

    // Solve created problem with method 2
    parallel_bfs::ParallelBFS par_bfs;
    measure(problem, par_bfs, "ParallelBFS");

    // Save problem
    std::filesystem::path root_path = get_build_dir_parent();
    std::filesystem::path output_path = root_path / "problem.yml";
    std::cout << "\n[INFO] Writing problem to " << output_path << "..." << std::endl;
    parallel_bfs::YAMLWriter writer;
    writer.write(problem, output_path);
    std::cout << "[INFO] Problem written." << std::endl;

    // // Read problem
    // const std::filesystem::path &input_path = output_path;
    // std::cout << "\n[INFO] Reading problem from " << input_path << "..." << std::endl;
    // parallel_bfs::YAMLReader<TreeState<std::uint32_t>, BasicTree<std::uint32_t>> reader;
    // auto problem_read = reader.read(input_path);
    // std::cout << "[INFO] Problem read." << std::endl;
    // std::cout << "[INFO] Writing problem to " << "problem_read.yml" << "..." << std::endl;
    // writer.write(problem_read, root_path / "problem_read.yml");
    // std::cout << "[INFO] Problem written." << std::endl;

    return 0;
}
