#include <filesystem>
#include <optional>
#include <iostream>
#include "generate.h"
#include "solve.h"


void show_help() {
    std::cout << "Usage: \n"
              << "\t--generate/-g <folder> : Generate problems in the given folder\n"
              << "\t--solve/-s <folder> : Solve problems from the given folder\n"
              << "\t--num-problems/-n <number> : Number of problems to generate and/or solve\n"
              << "\t--help/-h : Display this message\n"
              << "Example usage (must use either --generate or --solve, or both):\n"
              << "\t./main.out [(--generate <problems_gen_folder>)|(--solve <problems_sol_folder>)] [--num-problems <num_problems>]\n";
}


int main(int argc, char** argv) {
    std::optional<std::filesystem::path> gen_path;
    std::optional<std::filesystem::path> sol_path;
    std::optional<unsigned int> num_problems;

    if (argc == 1) {
        show_help();
        return 1;
    }

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];

        if (arg == "--help" || arg == "-h") {
            show_help();
            return 0;
        }

        if (arg == "--generate" || arg == "-g") {
            if (i + 1 < argc) {
                gen_path = argv[++i];
            } else {
                std::cerr << "No directory provided for " << arg << "\n";
                return 1;
            }
        }

        else if (arg == "--solve" || arg == "-s") {
            if (i + 1 < argc) {
                sol_path = argv[++i];
            } else {
                std::cerr << "No directory provided for " << arg << "\n";
                return 1;
            }
        }

        else if (arg == "--num-problems" || arg == "-n") {
            if (i + 1 < argc) {
                num_problems = std::stoi(argv[++i]);
            } else {
                std::cerr << "No number specified for " << arg << "\n";
                return 1;
            }
        }

        else {
            std::cerr << "Unknown argument: " << arg << "\n";
            return 1;
        }
    }

    if (!gen_path.has_value() && !sol_path.has_value()) {
        std::cerr << "No action specified. At least one of --generate or --solve flags must be provided.\n";
        return 1;
    }

    if (gen_path.has_value()) {
        if (!std::filesystem::exists(gen_path.value())) {
            std::filesystem::create_directories(gen_path.value());
            std::cout << "The directory '" << gen_path->string() << "' did not exist, so it has been created\n";
        }
        if (!std::filesystem::is_directory(gen_path.value())) {
            std::cerr << "The given path '" << gen_path->string() << "' is not a directory\n";
            return 1;
        }
        generate(*gen_path, num_problems);
    }

    if (sol_path.has_value()) {
        if (!std::filesystem::exists(sol_path.value()) || !std::filesystem::is_directory(sol_path.value())) {
            std::cerr << "The given path '" << sol_path->string() << "' does not exist or is not a directory\n";
            return 1;
        }
        solve(*sol_path, num_problems);
    }

    return 0;
}
