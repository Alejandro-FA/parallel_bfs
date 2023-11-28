//
// Created by Alejandro Fernández on 19/11/2023.
//

#ifndef PARALLEL_BFS_STATE_H
#define PARALLEL_BFS_STATE_H

template<typename T>
concept Searchable = std::regular<T>;

template<typename T>
concept Printable = requires(T t) { std::cout << t; };

#endif //PARALLEL_BFS_STATE_H
