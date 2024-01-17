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
        template<State State>
        void write(const Problem<State> &problem, const std::filesystem::path &output_path) const {
            std::ofstream output_file(output_path);
            if (!output_file) throw std::runtime_error("Could not create file to write problem.");

            write_metadata(output_file);
            write_initial(output_file, problem);
            write_goal(output_file, problem);
            write_transition_model(output_file, problem);
        }

    protected:
        void write_metadata(std::ostream& out) const {
            // ToDO: Decide if I want to write some metadata
        }

        template<State State>
        void write_initial(std::ostream& out, const Problem<State> &problem) const {
            write_node(out, "initial", problem.initial());
        }

        template<State State>
        void write_goal(std::ostream& out, const Problem<State> &problem) const {
            write_node(out, "goal_states", problem.goal_states());
        }

        template<State State>
        void write_transition_model(std::ostream& out, const Problem<State> &problem) const {
            // write_node(out, "transition_model", problem.transition_model()); // TODO: Uncomment this
        }

    private:
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
