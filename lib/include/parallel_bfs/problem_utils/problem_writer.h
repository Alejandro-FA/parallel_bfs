//
// Created by alejandro on 03/01/24.
//

#ifndef PARALLEL_BFS_PROBLEM_WRITER_H
#define PARALLEL_BFS_PROBLEM_WRITER_H

#include <iostream>
#include <filesystem>
#include <yaml-cpp/yaml.h>

namespace parallel_bfs {
    template<State State>
    class ProblemWriter {
    public:
        virtual ~ProblemWriter() = default;

        void write(const Problem<State> &problem, const std::filesystem::path &output_path) const {
            std::ofstream output_file(output_path);
            if (!output_file) throw std::runtime_error("Could not open file to write problem.");

            write_metadata(output_file);
            write_initial(output_file, problem);
            write_goal(output_file, problem);
            write_transition_model(output_file, problem);
        }

    protected:
        virtual void write_metadata(std::ostream& out) const = 0;

        virtual void write_initial(std::ostream& out, const Problem<State> &problem) const = 0;

        virtual void write_goal(std::ostream& out, const Problem<State> &problem) const = 0;

        virtual void write_transition_model(std::ostream& out, const Problem<State> &problem) const = 0;
    };


    template<typename T>
    concept ConvertibleToYAML = requires(const T &a, T &b, const YAML::Node &node) {
        { YAML::convert<T>::encode(a)} -> std::same_as<YAML::Node>;
        { YAML::convert<T>::decode(node, b)} -> std::same_as<bool>;
    };


    template<State State>
    class YAMLWriter : public ProblemWriter<State> {
    protected:
        void write_metadata(std::ostream& out) const override {
            // ToDO: Decide if I want to write some metadata
        }

        void write_initial(std::ostream& out, const Problem<State> &problem) const override {
            write_node(out, "initial", problem.initial());
        }

        void write_goal(std::ostream& out, const Problem<State> &problem) const override {
            write_node(out, "goal_states", problem.goal_states());
        }

        void write_transition_model(std::ostream& out, const Problem<State> &problem) const override {
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
