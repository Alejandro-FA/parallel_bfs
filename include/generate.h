//
// Created by alejandro on 01/02/24.
//

#ifndef PARALLEL_BFS_PROJECT_GENERATE_H
#define PARALLEL_BFS_PROJECT_GENERATE_H

#include <filesystem>
#include <iostream>
#include <optional>
#include <parallel_bfs/problem_utils.h>
#include <parallel_bfs/problems.h>
#include "generator_config.h"
#include "utils.h"


/**
 * @brief Converts an unsigned integer to a padded string.
 *
 * This function takes an unsigned integer and converts it to a string. It then pads the string with leading zeros
 * to match a specified width. The resulting string is returned.
 *
 * @param number The number to be converted to string.
 * @param pad_width The desired width of the resulting string. If the number has fewer digits, it will be padded with leading zeros.
 * @return The padded string representation of the input number.
 */
std::string to_padded_string(unsigned int number, int pad_width) {
    std::stringstream ss;
    ss << std::setw(pad_width) << std::setfill('0') << number;
    return ss.str();
}


/**
 * @brief Generates random problems and writes them to files.
 *
 * This function generates random problems using a tree generator and writes them to individual files in the specified output directory.
 * The number of problems to generate can be specified, as well as the configuration for the tree generator.
 *
 * @param output_dir The directory where the generated problems will be written.
 * @param num_problems The number of problems to generate. If not specified, 1 problem will be generated.
 * @param config The configuration for the tree generator. If not specified, default values will be used.
 * @note This function does NOT validate if @output_dir is a valid directory.
 */
void generate(const std::filesystem::path &output_dir, std::optional<unsigned int> num_problems, std::optional<BasicTreeGeneratorConfig> config) {
    using namespace parallel_bfs;
    unsigned int n = num_problems.value_or(1);
    BasicTreeGeneratorConfig c = config.value_or(BasicTreeGeneratorConfig::simple());

    const YAMLWriter writer;
    auto tree_generator =
        c.avg_actions.has_value()
        ? BasicTreeGenerator<std::uint32_t>{c.max_depth, c.num_goals, c.avg_actions.value(), c.max_actions}
        : BasicTreeGenerator<std::uint32_t>{c.max_depth, c.num_goals, c.min_actions.value(), c.max_actions};

    std::cout << "[INFO] Generating " << n << " random problems and writing them to " << output_dir << "..." << std::endl;
    auto bar = SimpleProgressBar(n * 2, true);

    for (unsigned int i = 0; i < n; ++i) {
        // Create random problem
        bar.set_status("Creating random problem " + std::to_string(i));
        const auto problem = tree_generator.make_problem();
        bar.tick();

        // Build file path to write the problem to
        std::string padded_string = to_padded_string(i, static_cast<int>(std::to_string(n-1).length()));
        std::string filename {"problem_" + padded_string};
        filename += writer.file_extension;
        const auto output_path = output_dir / filename;

        // Write the problem
        bar.set_status("Writing " + output_path.filename().string());
        writer.write(problem, output_path);
        bar.tick();
    }
}

#endif //PARALLEL_BFS_PROJECT_GENERATE_H
