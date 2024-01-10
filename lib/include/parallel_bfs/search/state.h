//
// Created by Alejandro Fernández on 19/11/2023.
//

#ifndef PARALLEL_BFS_STATE_H
#define PARALLEL_BFS_STATE_H

namespace parallel_bfs {
    template<typename T>
    concept Hashable = requires(const T &a) {
        { std::hash<T>{}(a) } -> std::convertible_to<std::size_t>;
    };

    template<typename T>
    concept Searchable = std::regular<T> && Hashable<T>;

    template<typename T>
    concept Printable = requires(T t) { std::cout << t; };
}

#endif //PARALLEL_BFS_STATE_H
