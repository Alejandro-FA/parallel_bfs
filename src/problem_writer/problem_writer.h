//
// Created by alejandro on 03/01/24.
//

#ifndef PARALLEL_BFS_PROBLEM_WRITER_H
#define PARALLEL_BFS_PROBLEM_WRITER_H

#include <iostream>
#include <filesystem>
#include <utility>
#include "../problem/problem.h"

template<Printable T>
class ProblemWriter {
public:
    virtual ~ProblemWriter() = default;

    bool write(const Problem<T> &problem) const {
        // TODO: Complete algorithm
        try {
            write_metadata();
            write_initial();
            write_goal();
            write_child_generator();
        } catch (const std::exception& e) {
            std::cout << "Error when writing file '" << _output_path << "'\n";
            return false;
        }
    }

protected:
    std::filesystem::path _output_path;

    explicit ProblemWriter(std::filesystem::path output_path) : _output_path{std::move(output_path)} {}

    virtual void write_metadata() = 0;

    virtual void write_initial() = 0;

    virtual void write_goal() = 0;

    virtual void write_child_generator() = 0;
};

#endif //PARALLEL_BFS_PROBLEM_WRITER_H
