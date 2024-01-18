//
// Created by alejandro on 03/01/24.
//

#ifndef PARALLEL_BFS_PROBLEM_READER_H
#define PARALLEL_BFS_PROBLEM_READER_H

#include <filesystem>
#include "problem_factory.h"

namespace parallel_bfs {
    template<State State, std::derived_from<BaseTransitionModel<State>> TM>
    class YAMLReader : public ProblemFactory<State, TM> {
    public:
        explicit YAMLReader(std::filesystem::path input_path) : _input_path{std::move(input_path)} {}

        // TODO:

    private:
        std::filesystem::path _input_path;
    };
}

#endif //PARALLEL_BFS_PROBLEM_READER_H
