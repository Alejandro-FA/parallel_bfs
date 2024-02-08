//
// Created by alejandro on 03/02/24.
//

#ifndef PARALLEL_BFS_PROJECT_GENERATOR_CONFIG_H
#define PARALLEL_BFS_PROJECT_GENERATOR_CONFIG_H

#include <filesystem>
#include <fstream>
#include <string>
#include <parallel_bfs/problems.h>
#include <yaml-cpp/yaml.h>


struct BasicTreeGeneratorConfig {
    using state_t = std::uint32_t;
    unsigned int max_depth;
    unsigned int num_goals;
    unsigned int max_actions;
    double avg_actions;

    [[nodiscard]] static BasicTreeGeneratorConfig simple() {
        return BasicTreeGeneratorConfig{4, 10, 3, 1.5};
    }

    [[nodiscard]] static BasicTreeGeneratorConfig medium() {
        return BasicTreeGeneratorConfig{8, 20, 6, 3.0};
    }

    [[nodiscard]] static BasicTreeGeneratorConfig complex() {
        return BasicTreeGeneratorConfig{9, 30, 7, 5.0};
    }
};


[[nodiscard]] BasicTreeGeneratorConfig parse_config(const std::string &file_name) noexcept(false) {
    // Check if the file exists
    const std::filesystem::path file_path{file_name};
    if (!std::filesystem::exists(file_path))
        throw std::filesystem::filesystem_error("cannot parse config file", file_path, std::make_error_code(std::errc::no_such_file_or_directory));

    // Parse the file
    YAML::Node node = YAML::LoadFile(file_path);
    auto max_depth = node["max_depth"].as<unsigned int>(1);
    auto num_goals = node["num_goals"].as<unsigned int>(1);
    auto max_actions = node["max_actions"].as<unsigned int>(1);
    auto avg_actions = node["avg_actions"].as<double>(1);

    return BasicTreeGeneratorConfig{max_depth, num_goals, max_actions, avg_actions};
}

#endif //PARALLEL_BFS_PROJECT_GENERATOR_CONFIG_H
