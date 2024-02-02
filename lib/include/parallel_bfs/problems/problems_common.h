//
// Created by alejandro on 24/01/24.
//

#ifndef PARALLEL_BFS_PROJECT_PROBLEMS_COMMON_H
#define PARALLEL_BFS_PROJECT_PROBLEMS_COMMON_H

#include <concepts>

namespace parallel_bfs::detail {
    template<typename T>
    concept UnsignedInteger = std::same_as<T, unsigned short> || std::same_as<T, unsigned int> ||
    std::same_as<T, unsigned long> || std::same_as<T, unsigned long long>;
}

#endif //PARALLEL_BFS_PROJECT_PROBLEMS_COMMON_H
