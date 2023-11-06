cmake_minimum_required(VERSION 3.26)
project(parallel_bfs)

set(CMAKE_CXX_STANDARD 23)

add_executable(parallel_bfs src/main.cpp
        src/problem.h
        src/problem_generator.h
        src/node.h
        src/bfs/bfs.h
        src/state.h
        src/utils.h
        src/bfs/sync_bfs.h
        src/bfs/parallel_bfs.h
        src/bfs/base_bfs.h
)
