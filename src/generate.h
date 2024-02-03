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
* Function: generate
* ----------------------
* Generates random problems and writes them to @output_dir.
*
* @param output_dir: a path to the directory where problems will be written.
* @param num_problems: the number of problems to generate.
* @param config: the configuration for the problem generator.
*
* NOTE: This function does NOT validate if @output_dir is a valid directory.
*/
void generate(const std::filesystem::path &output_dir, std::optional<unsigned int> num_problems, std::optional<BasicTreeGeneratorConfig> config) {
    using namespace parallel_bfs;
    unsigned int n = num_problems.value_or(1);
    BasicTreeGeneratorConfig c = config.value_or(BasicTreeGeneratorConfig::simple());

    BasicTreeGenerator<std::uint32_t> tree_generator{c.max_depth, c.num_goals, c.max_actions, c.avg_actions};
    const YAMLWriter writer;

    for (unsigned int i = 0; i < n; ++i) {
        // Create random problem
        std::cout << "\n[INFO] Creating random problem " << i + 1 << "..." << std::flush;
        auto [problem, elapsed_time] = invoke_and_time([&]{ return tree_generator.make_problem(); });
        std::cout << " [" << elapsed_time.as_seconds() << " s]" << std::endl;

        // Build file path to write the problem to
        std::string padded_string = to_padded_string(i, static_cast<int>(std::to_string(n-1).length()));
        std::string filename {"problem_" + padded_string};
        filename += writer.file_extension;
        const std::filesystem::path output_path = output_dir / filename;

        // Write the problem
        std::cout << "[INFO] Writing problem to '" << output_path.string() << "'..." << std::flush;
        elapsed_time = invoke_and_time([&]{ writer.write(problem, output_path); });
        std::cout << " [" << elapsed_time.as_seconds() << " s]" << std::endl;
    }
}

#endif //PARALLEL_BFS_PROJECT_GENERATE_H
