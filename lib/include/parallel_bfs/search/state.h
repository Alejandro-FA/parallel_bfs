//
// Created by Alejandro Fernández on 19/11/2023.
//

#ifndef PARALLEL_BFS_STATE_H
#define PARALLEL_BFS_STATE_H

namespace {
    template<typename T>
    concept Printable = requires(const T &a) { std::cout << a; };

    template<typename T>
    concept Hashable = requires(const T &a) {
        requires std::regular<T>; // Also includes std::equality_comparable<T>
        { std::hash<T>{}(a) } -> std::convertible_to<std::size_t>;
    };
}

namespace parallel_bfs {
    template<typename T> concept State = Hashable<T> && Printable<T>;
}

#endif //PARALLEL_BFS_STATE_H
