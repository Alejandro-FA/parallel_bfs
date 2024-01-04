cmake_minimum_required(VERSION 3.24)
project(parallel_bfs)

set(CMAKE_CXX_STANDARD 23)

# If you only include this third party in PRIVATE source files, you do not need to install it
# when your main project gets installed.
set(JSON_Install OFF CACHE INTERNAL "")

add_subdirectory(nlohmann_json)

add_executable(parallel_bfs src/main.cpp
        src/utils.h
        src/bfs/sync_bfs.h
        src/bfs/parallel_bfs.h
        src/problem/problem.h
        src/problem/transition_model.h
        src/problem/transition_models/basic_graph.h
        src/problem/transition_models/basic_tree.h
        src/node.h
        src/state/state.h
        src/state/tree_state.h
        src/problem_factory/problem_factory.h
        src/problem_factory/builders/random_builder.h
        src/problem_factory/builders/basic_graph_builder.h
        src/problem_factory/builders/basic_tree_builder.h
        src/problem_factory/readers/problem_reader.h
        src/problem_writer/problem_writer.h
        src/problem_writer/writers/json_writer.h
        src/problem_parser/problem_parser.h
        src/problem_parser/parsers/json_parser.h
)

target_link_libraries(parallel_bfs PRIVATE nlohmann_json::nlohmann_json)

set(is_clang "$<CXX_COMPILER_ID:Clang,AppleClang>")
add_compile_options("-Wall -Wextra -Wpedantic $<${is_clang}:-Wthread-safety>")

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
        --build-and-test "${CMAKE_SOURCE_DIR}" "${CMAKE_BINARY_DIR}/cmake-build-asan-tests"
        --build-generator ${CMAKE_GENERATOR}
        --build-makeprogram ${CMAKE_MAKE_PROGRAM}
        --build-project "parallel_bfs"
        --build-options -DCMAKE_CXX_FLAGS=${asan_flags} ${common_build_opts}
        --test-command "parallel_bfs"
)

set(tsan_flags "-fsanitize=thread -fno-omit-frame-pointer -O2")
add_test(tsan ${CMAKE_CTEST_COMMAND}
        --build-and-test "${CMAKE_SOURCE_DIR}" "${CMAKE_BINARY_DIR}/cmake-build-tsan-tests"
        --build-generator ${CMAKE_GENERATOR}
        --build-makeprogram ${CMAKE_MAKE_PROGRAM}
        --build-project "parallel_bfs"
        --build-options -DCMAKE_CXX_FLAGS=${tsan_flags} ${common_build_opts}
        --test-command "parallel_bfs"
)

set(sanitizers_env_options
        ASAN_OPTIONS=detect_leaks=1:detect_stack_use_after_return=false
        LSAN_OPTIONS=suppressions=${CMAKE_SOURCE_DIR}/MyLSan.supp
        UBSAN_OPTIONS=halt_on_error=1
        TSAN_OPTIONS=halt_on_error=1
)
set_tests_properties(asan_lsan_ubsan tsan PROPERTIES ENVIRONMENT "${sanitizers_env_options}")
