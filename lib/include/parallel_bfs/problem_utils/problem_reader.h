//
// Created by alejandro on 03/01/24.
//

#ifndef PARALLEL_BFS_PROBLEM_READER_H
#define PARALLEL_BFS_PROBLEM_READER_H

#include <filesystem>
#include "problem_factory.h"

namespace parallel_bfs {
    template<State State>
    class ProblemReader : public ProblemFactory<State> {
    public:
        explicit ProblemReader(std::filesystem::path input_path) : _input_path{std::move(input_path)} {}

        // TODO:

    private:
        std::filesystem::path _input_path;
    };
}

#endif //PARALLEL_BFS_PROBLEM_READER_H
