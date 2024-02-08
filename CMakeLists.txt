cmake_minimum_required(VERSION 3.24)
project(parallel_bfs_project
        VERSION 0.3
        DESCRIPTION "Project for testing parallel strategies for Best First Search"
        LANGUAGES CXX
        HOMEPAGE_URL "https://github.com/Alejandro-FA/parallel_bfs"
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

################### Compile options #####################
set(is_clang "$<CXX_COMPILER_ID:Clang,AppleClang>")
add_compile_options(-Wall -Wextra -Wpedantic "$<${is_clang}:-Wthread-safety>")

##################### Dependencies ######################
include(FetchContent)
FetchContent_Declare(
        indicators
        GIT_REPOSITORY https://github.com/p-ranav/indicators.git
        GIT_TAG v2.3
)
FetchContent_MakeAvailable(indicators)

add_subdirectory(lib) # Custom library for parallel BFS

##################### Executables #######################
add_executable(main
        src/main.cpp
        include/generate.h
        include/generator_config.h
        include/solve.h
        include/solver.h
        include/statistics.h
        include/utils.h
)

target_link_libraries(main PRIVATE parallel_bfs)
target_link_libraries(main PRIVATE indicators::indicators)
set_target_properties(main PROPERTIES SUFFIX ".out")

####################### Testing #########################
enable_testing()

set(common_build_opts
        -DCMAKE_BUILD_TYPE=Debug
        -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
        -DCMAKE_OSX_SYSROOT=${CMAKE_OSX_SYSROOT}
        -DCMAKE_COLOR_DIAGNOSTICS=${CMAKE_COLOR_DIAGNOSTICS}
)

set(asan_flags "-fsanitize=address,undefined -fno-omit-frame-pointer -Og")
add_test(asan_lsan_ubsan ${CMAKE_CTEST_COMMAND}
        --build-and-test "${CMAKE_SOURCE_DIR}" "${CMAKE_SOURCE_DIR}/cmake-build-tests-asan"
        --build-generator ${CMAKE_GENERATOR}
        --build-makeprogram ${CMAKE_MAKE_PROGRAM}
        --build-project "parallel_bfs"
        --build-options -DCMAKE_CXX_FLAGS=${asan_flags} ${common_build_opts}
        --test-command main.out --config=${CMAKE_SOURCE_DIR}/config/test_tree.yaml
)

set(tsan_flags "-fsanitize=thread -fno-omit-frame-pointer -O2")
add_test(tsan ${CMAKE_CTEST_COMMAND}
        --build-and-test "${CMAKE_SOURCE_DIR}" "${CMAKE_SOURCE_DIR}/cmake-build-tests-tsan"
        --build-generator ${CMAKE_GENERATOR}
        --build-makeprogram ${CMAKE_MAKE_PROGRAM}
        --build-project "parallel_bfs"
        --build-options -DCMAKE_CXX_FLAGS=${tsan_flags} ${common_build_opts}
        --test-command main.out --config=${CMAKE_SOURCE_DIR}/config/test_tree.yaml
)

set(sanitizers_env_options
        ASAN_OPTIONS=detect_leaks=1:detect_stack_use_after_return=false
        LSAN_OPTIONS=suppressions=${CMAKE_SOURCE_DIR}/MyLSan.supp
        UBSAN_OPTIONS=halt_on_error=1
        TSAN_OPTIONS=halt_on_error=1
)
set_tests_properties(asan_lsan_ubsan tsan PROPERTIES ENVIRONMENT "${sanitizers_env_options}")
