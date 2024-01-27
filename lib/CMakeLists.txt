################## External libraries ###################
include(FetchContent)

FetchContent_Declare(
        yaml-cpp
        GIT_REPOSITORY https://github.com/jbeder/yaml-cpp.git
        master # Can be a tag (GIT_TAG x.y.z), a commit hash, or a branch name (master)
)
FetchContent_MakeAvailable(yaml-cpp)

####################### Sources #########################
add_library(parallel_bfs INTERFACE
        include/parallel_bfs/search.h
        include/parallel_bfs/problem_utils.h
        include/parallel_bfs/search/node.h
        include/parallel_bfs/search/parallel_bfs.h
        include/parallel_bfs/search/problem.h
        include/parallel_bfs/search/state.h
        include/parallel_bfs/search/sync_bfs.h
        include/parallel_bfs/search/transition_model.h
        include/parallel_bfs/search/utils.h
        include/parallel_bfs/problem_utils/problem_factory.h
        include/parallel_bfs/problem_utils/problem_reader.h
        include/parallel_bfs/problem_utils/problem_writer.h
)

target_link_libraries(parallel_bfs INTERFACE yaml-cpp::yaml-cpp)

target_include_directories(parallel_bfs INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)

set_target_properties(parallel_bfs PROPERTIES LINKER_LANGUAGE CXX)