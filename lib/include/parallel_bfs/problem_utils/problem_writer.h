//
// Created by alejandro on 03/01/24.
//

#ifndef PARALLEL_BFS_PROBLEM_WRITER_H
#define PARALLEL_BFS_PROBLEM_WRITER_H

#include <iostream>
#include <filesystem>
#include <yaml-cpp/yaml.h>
#include "type_name.h"


namespace parallel_bfs {
    template<typename T>
    concept ConvertibleToYAML = requires(const T &a, T &b, const YAML::Node &node, YAML::Emitter &emitter) {
        { YAML::convert<T>::encode(a)} -> std::same_as<YAML::Node>;
        { YAML::convert<T>::decode(node, b)} -> std::same_as<bool>;
        { emitter << a } -> std::same_as<YAML::Emitter&>;
    };


    class YAMLWriter {
    public:
        template<Searchable State, std::derived_from<BaseTransitionModel<State>> TM>
        requires ConvertibleToYAML<State> && ConvertibleToYAML<TM>
        void write(const Problem<State, TM> &problem, const std::filesystem::path &output_path) const {
            std::ofstream output_file{output_path};
            if (!output_file) throw std::runtime_error("Could not create file to write problem.");

            YAML::Emitter emitter;
            emitter << YAML::BeginMap;
            emitter << YAML::Key << "state type" << YAML::Value << std::string{type_name<State>()};
            emitter << YAML::Key << "transition model type" << YAML::Value << std::string{type_name<TM>()};
            emitter << YAML::Key << "initial" << YAML::Value << problem.initial();
            emitter << YAML::Key << "goal states" << YAML::Value << problem.goal_states();
            emitter << YAML::Key << "transition model" << YAML::Value << problem.transition_model();
            emitter << YAML::EndMap;

            output_file << emitter.c_str() << '\n';
        }
    };
}



namespace YAML {
    // Template specialization for std::unordered_set<T>. Needed for problem.goal_states().
    template<parallel_bfs::ConvertibleToYAML T>
    struct convert<std::unordered_set<T>> {
        static Node encode(const std::unordered_set<T> &rhs) {
            Node node(NodeType::Sequence);
            std::for_each(rhs.cbegin(), rhs.cend(), [&node](const T &v) { node.push_back(v); });
            return node;
        }

        static bool decode(const Node &node, std::unordered_set<T> &rhs) {
            if (!node.IsSequence()) return false;
            std::for_each(node.begin(), node.end(), [&rhs](const auto &v) { rhs.insert(v.template as<T>()); });
            return true;
        }
    };


    template<parallel_bfs::ConvertibleToYAML T>
    Emitter& operator << (Emitter& out, const std::unordered_set<T> s) {
        out << YAML::BeginSeq;
        std::for_each(s.cbegin(), s.cend(), [&out](const T &v) { out << v; });
        out << YAML::EndSeq;
        return out;
    }
}

#endif //PARALLEL_BFS_PROBLEM_WRITER_H
