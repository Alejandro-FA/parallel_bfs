//
// Created by alejandro on 03/01/24.
//

#ifndef PARALLEL_BFS_PROBLEM_WRITER_H
#define PARALLEL_BFS_PROBLEM_WRITER_H

#include <iostream>
#include <filesystem>
#include <yaml-cpp/yaml.h>


namespace parallel_bfs {
    template<typename T>
    concept ConvertibleToYAML = requires(const T &a, T &b, const YAML::Node &node) {
        { YAML::convert<T>::encode(a)} -> std::same_as<YAML::Node>;
        { YAML::convert<T>::decode(node, b)} -> std::same_as<bool>;
    };


    class YAMLWriter {
    public:
        template<State State, std::derived_from<BaseTransitionModel<State>> TM>
        void write(const Problem<State, TM> &problem, const std::filesystem::path &output_path) const {
            std::ofstream output_file(output_path);
            if (!output_file) throw std::runtime_error("Could not create file to write problem.");

            write_metadata(output_file);
            write_node(output_file, "initial", problem.initial());
            write_node(output_file, "goal_states", problem.goal_states());
            write_node(output_file, "transition_model", problem.transition_model());
        }

    private:
        void write_metadata(std::ostream& out) const {
            // ToDO: Decide if I want to write some metadata
        }

        void write_node(std::ostream& out, const std::string &key, const ConvertibleToYAML auto &data) const {
            YAML::Node node;
            node[key] = data;

            YAML::Emitter emitter;
            emitter << node;

            out << emitter.c_str() << '\n';
        }
    };
}



namespace YAML {
    // Template specialization for std::unordered_set<T>. Needed for problem.goal_states().
    template<parallel_bfs::ConvertibleToYAML T>
    struct convert<std::unordered_set<T>> {
        static Node encode(const std::unordered_set<T> &rhs) {
            Node node;
            std::for_each(rhs.cbegin(), rhs.cend(), [&node](const T &v) { node.push_back(v); });
            return node;
        }

        static bool decode(const Node &node, std::unordered_set<T> &rhs) {
            if (!node.IsSequence()) return false;
            std::for_each(node.begin(), node.end(), [&rhs](const auto &v) { rhs.insert(v.template as<T>()); });
            return true;
        }
    };
}

#endif //PARALLEL_BFS_PROBLEM_WRITER_H
