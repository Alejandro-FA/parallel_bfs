#include <iostream>
#include <fstream>
#include <format>
#include <parallel_bfs/search.h>
#include <parallel_bfs/problem_utils.h>
#include "example_problems/basic_graph/basic_graph_generator.h"
#include "example_problems/basic_tree/basic_tree_generator.h"
#include "utils.h"


int main() {
    // Setup
    BasicGraphGenerator<std::uint32_t> graph_generator{1'000'000, 4};
    BasicTreeGenerator<std::uint32_t> tree_generator{9, 5, 7, 5.0};
    parallel_bfs::SyncBFS sync_bfs;
    parallel_bfs::ParallelBFSTasks par_bfs_tasks;
    parallel_bfs::ParallelBFSAsync par_bfs_async;

    const parallel_bfs::YAMLWriter writer;
    const std::vector<unsigned int> seeds{87, 48};

    for (std::size_t i = 0; i < seeds.size(); ++i) {
        // Create random problem
        std::string message = std::format("Creating random problem {}", i + 1);
        parallel_bfs::Problem problem = run_and_measure([&]{ return tree_generator.make_problem(seeds[i]); }, message);
        std::cout << "\n" << problem << "\n";

        // Solve created problem with method 1
        auto solution = run_and_measure([&] { return sync_bfs(problem); }, "SyncBFS");
        log_solution(solution.get());

        // Solve created problem with method 2
        solution = run_and_measure([&] { return par_bfs_tasks(problem); }, "ParallelBFSTasks");
        log_solution(solution.get());

        // Solve created problem with method 3
        solution = run_and_measure([&] { return par_bfs_async(problem); }, "ParallelBFSAsync");
        log_solution(solution.get());

        // Save problem
        const std::filesystem::path output_path = get_build_dir_parent() / "problem.yml";
        message = std::format("Writing problem to {}", output_path.string());
        run_and_measure([&] { return writer.write(problem, output_path); }, message);

        std::cout << "\n----------------------------------------\n";
    }

    // // Example on how to read a problem
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
