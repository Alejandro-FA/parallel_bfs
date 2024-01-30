//
// Created by alejandro on 30/01/24.
//

#ifndef PARALLEL_BFS_PROJECT_UTILS_H
#define PARALLEL_BFS_PROJECT_UTILS_H

#include <filesystem>

std::filesystem::path get_build_dir_parent() {
    std::filesystem::path current_path = std::filesystem::current_path();
    if (current_path.filename().string().starts_with("cmake-build")) {
        current_path = current_path.parent_path();
    }
    return current_path;
}

#endif //PARALLEL_BFS_PROJECT_UTILS_H
