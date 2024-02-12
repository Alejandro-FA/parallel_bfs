cmake_minimum_required(VERSION 3.24)
project(parallel_bfs_project
        VERSION 0.3
        DESCRIPTION "Project for testing parallel strategies for Best First Search"
        LANGUAGES CXX
        HOMEPAGE_URL "https://github.com/Alejandro-FA/parallel_bfs"
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

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

################### Compile options #####################
set(is_clang "$<CXX_COMPILER_ID:Clang,AppleClang>")
target_compile_options(main PRIVATE -Wall -Wextra -Wpedantic "$<${is_clang}:-Wthread-safety>")

find_package(Threads QUIET)
if (NOT Threads_FOUND)
    message(FATAL_ERROR "Threads library not found. Please check the README.md file for more information.")
else()
    if (CMAKE_USE_PTHREADS_INIT)
        message(STATUS "Pthread library found.")
    else()
        message(STATUS "Supported thread library (different than pthreads) found.")
    endif()
    if(NOT DEFINED CMAKE_THREAD_LIBS_INIT OR CMAKE_THREAD_LIBS_INIT STREQUAL "")
        message(STATUS "Special flags for thread library are not required.")
    else()
        message(STATUS "Special flags for thread library required: ${CMAKE_THREAD_LIBS_INIT}")
    endif()
    set(THREADS_PREFER_PTHREAD_FLAG TRUE)
    set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
    target_link_libraries(main PRIVATE Threads::Threads)
endif()

find_package(TBB QUIET)
if (NOT TBB_FOUND)
    message(FATAL_ERROR "TBB library not found. Please check the README.md file for more information.")
else()
    message(STATUS "TBB found at ${TBB_DIR}")
    message(STATUS "TBB version: ${TBB_VERSION}")
    target_link_libraries(main PRIVATE TBB::tbb)
endif()

#find_package(OpenMP REQUIRED)
#if (OpenMP_CXX_FOUND)
#    target_link_libraries(main PRIVATE OpenMP::OpenMP_CXX)
#endif()

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
        --build-and-test "${CMAKE_SOURCE_DIR}" "${CMAKE_SOURCE_DIR}/cmake-build-asan"
        --build-generator ${CMAKE_GENERATOR}
        --build-makeprogram ${CMAKE_MAKE_PROGRAM}
        --build-project "parallel_bfs"
        --build-options -DCMAKE_CXX_FLAGS=${asan_flags} ${common_build_opts}
        --test-command main.out problems --config=${CMAKE_SOURCE_DIR}/config/simple_tree.yaml -n 10
)

set(tsan_flags "-fsanitize=thread -fno-omit-frame-pointer -O2")
add_test(tsan ${CMAKE_CTEST_COMMAND}
        --build-and-test "${CMAKE_SOURCE_DIR}" "${CMAKE_SOURCE_DIR}/cmake-build-tsan"
        --build-generator ${CMAKE_GENERATOR}
        --build-makeprogram ${CMAKE_MAKE_PROGRAM}
        --build-project "parallel_bfs"
        --build-options -DCMAKE_CXX_FLAGS=${tsan_flags} ${common_build_opts}
        --test-command main.out problems --config=${CMAKE_SOURCE_DIR}/config/simple_tree.yaml -n 10
)

set(sanitizers_env_options
        ASAN_OPTIONS=detect_leaks=1:detect_stack_use_after_return=false
        LSAN_OPTIONS=suppressions=${CMAKE_SOURCE_DIR}/MyLSan.supp
        UBSAN_OPTIONS=halt_on_error=1
        TSAN_OPTIONS=halt_on_error=1
)
set_tests_properties(asan_lsan_ubsan tsan PROPERTIES ENVIRONMENT "${sanitizers_env_options}")
