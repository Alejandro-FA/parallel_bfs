//
// Created by alejandro on 03/01/24.
//

#ifndef PARALLEL_BFS_PROBLEM_READER_H
#define PARALLEL_BFS_PROBLEM_READER_H

#include "../../problem_parser/problem_parser.h"

class ProblemReader {
public:
    virtual ~ProblemReader() = default;

protected:
    ProblemParser _parser;

    explicit ProblemReader(ProblemParser parser) : _parser{parser} {}
};

#endif //PARALLEL_BFS_PROBLEM_READER_H
