//
// Created by alejandro on 03/02/24.
//

#ifndef PARALLEL_BFS_PROJECT_GENERATOR_CONFIG_H
#define PARALLEL_BFS_PROJECT_GENERATOR_CONFIG_H

#include <filesystem>
#include <fstream>
#include <string>
#include <optional>
#include <yaml-cpp/yaml.h>


struct BasicTreeGeneratorConfig {
    using state_t = std::uint32_t;
    unsigned int max_depth{1};
    unsigned int num_goals{1};
    std::optional<double> avg_actions{std::nullopt};
    std::optional<unsigned int> min_actions{std::nullopt};
    unsigned int max_actions{1};

    [[nodiscard]] static BasicTreeGeneratorConfig simple() {
        return BasicTreeGeneratorConfig{4, 10, 1.5, 0, 3};
    }

    [[nodiscard]] static BasicTreeGeneratorConfig medium() {
        return BasicTreeGeneratorConfig{8, 20, 3.0, 0, 6};
    }

    [[nodiscard]] static BasicTreeGeneratorConfig complex() {
        return BasicTreeGeneratorConfig{9, 30, 5.0, 0, 7};
    }
};


[[nodiscard]] BasicTreeGeneratorConfig parse_config(const std::string &file_name) noexcept(false) {
    // Check if the file exists
    const std::filesystem::path file_path{file_name};
    if (!std::filesystem::exists(file_path))
        throw std::filesystem::filesystem_error("cannot parse config file", file_path, std::make_error_code(std::errc::no_such_file_or_directory));

    // Parse the file
    YAML::Node node = YAML::LoadFile(file_path);
    auto max_depth = node["max_depth"].as<unsigned int>();
    auto num_goals = node["num_goals"].as<unsigned int>();
    auto avg_actions = node["avg_actions"].as<std::optional<double>>(std::nullopt);
    auto min_actions = node["min_actions"].as<std::optional<unsigned int>>(std::nullopt);
    auto max_actions = node["max_actions"].as<unsigned int>();

    // Validate the parsed values
    if (avg_actions.has_value() && min_actions.has_value())
        throw std::invalid_argument("Both avg_actions and min_actions are specified. Only one of them can be used.");
    if (!avg_actions.has_value() && !min_actions.has_value())
        throw std::invalid_argument("Either avg_actions or min_actions must be specified.");

    return BasicTreeGeneratorConfig{max_depth, num_goals, avg_actions, min_actions, max_actions};
}


namespace YAML {
    template<typename T>
    struct convert<std::optional<T>> {
        static Node encode(const std::optional<T> &rhs) {
            if (rhs.has_value()) return YAML::convert<T>::encode(rhs.value());
            return YAML::Node{};
        }

        static bool decode(const Node &node, std::optional<T> &rhs) {
            if (node.IsDefined()) {
                T value;
                if (node.IsNull()) {
                    rhs = std::nullopt;
                    return true;
                } else if (YAML::convert<T>::decode(node, value)) {
                    rhs = value;
                    return true;
                }
            }
            return false;
        }
    };
}

#endif //PARALLEL_BFS_PROJECT_GENERATOR_CONFIG_H
