cmake_minimum_required(VERSION 3.24)
project(parallel_bfs)

set(CMAKE_CXX_STANDARD 23)

add_executable(parallel_bfs src/main.cpp
        src/utils.h
        src/state.h
        src/node.h
        src/bfs/bfs.h
        src/bfs/sync_bfs.h
        src/bfs/parallel_bfs.h
        src/problem/problem.h
        src/problem/random_graph.h
        src/problem_factory/problem_factory.h
        src/problem_factory/random_graph_factory.h
)

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
