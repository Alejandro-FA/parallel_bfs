################## External libraries ###################
include(FetchContent)

FetchContent_Declare(
        yaml-cpp
        GIT_REPOSITORY https://github.com/jbeder/yaml-cpp.git
        GIT_TAG c67d701ad81da65a576efd3341634f8cd3892c9b # Can be a tag (GIT_TAG x.y.z), a commit hash, or a branch name (master)
)
FetchContent_MakeAvailable(yaml-cpp)

####################### Sources #########################
add_library(parallel_bfs INTERFACE
        include/parallel_bfs/search.h
        include/parallel_bfs/search/search_strategies/async_bfs.h
        include/parallel_bfs/search/search_strategies/async_start_bfs.h
        include/parallel_bfs/search/search_strategies/bfs.h
        include/parallel_bfs/search/search_strategies/tasks_bfs.h
        include/parallel_bfs/search/search_strategies/sync_bfs.h
        include/parallel_bfs/search/node.h
        include/parallel_bfs/search/problem.h
        include/parallel_bfs/search/state.h
        include/parallel_bfs/search/transition_model.h
        include/parallel_bfs/problem_utils.h
        include/parallel_bfs/problem_utils/problem_factory.h
        include/parallel_bfs/problem_utils/problem_reader.h
        include/parallel_bfs/problem_utils/problem_writer.h
        include/parallel_bfs/problem_utils/type_name.h
        include/parallel_bfs/problems.h
        include/parallel_bfs/problems/basic_graph/basic_graph.h
        include/parallel_bfs/problems/basic_graph/basic_graph_generator.h
        include/parallel_bfs/problems/basic_tree/basic_tree.h
        include/parallel_bfs/problems/basic_tree/basic_tree_generator.h
        include/parallel_bfs/problems/basic_tree/tree_state.h
        include/parallel_bfs/problems/problems_common.h
)

target_link_libraries(parallel_bfs INTERFACE yaml-cpp::yaml-cpp)

target_include_directories(parallel_bfs INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)

set_target_properties(parallel_bfs PROPERTIES LINKER_LANGUAGE CXX)