//
// Created by alejandro on 03/01/24.
//

#ifndef PARALLEL_BFS_PROBLEM_READER_H
#define PARALLEL_BFS_PROBLEM_READER_H

#include <fstream>
#include <filesystem>
#include <string>
#include <yaml-cpp/yaml.h>
#include "problem_writer.h"

namespace parallel_bfs {
    template<Searchable State, std::derived_from<BaseTransitionModel<State>> TM>
    requires ConvertibleToYAML<State> && ConvertibleToYAML<TM>
    class YAMLReader {
    public:
        [[nodiscard]] Problem<State, TM> read(const std::filesystem::path &input_path) const {
            std::ifstream input_file{input_path};
            if (!input_file) throw std::runtime_error("Could not read file " + input_path.string());

            YAML::Node node = YAML::LoadFile(input_path);
            State initial = node["initial"].as<State>();
            std::unordered_set<State> goal_states = node["goal states"].as<std::unordered_set<State>>();
            TM transition_model = node["transition model"].as<TM>();

            return Problem<State, TM>{initial, std::move(goal_states), std::move(transition_model)};
        }

        std::string file_extension{".yaml"};
    };
}

#endif //PARALLEL_BFS_PROBLEM_READER_H
