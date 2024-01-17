################## External libraries ###################
include(FetchContent)

#FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz)
#set(JSON_ImplicitConversions OFF CACHE INTERNAL "")
#FetchContent_MakeAvailable(json)

FetchContent_Declare(
        yaml-cpp
        GIT_REPOSITORY https://github.com/jbeder/yaml-cpp.git
        GIT_TAG 0.8.0 # Can be a tag (yaml-cpp-x.x.x), a commit hash, or a branch name (master)
)
FetchContent_MakeAvailable(yaml-cpp)

####################### Sources #########################
add_library(parallel_bfs
        include/parallel_bfs/search.h
        include/parallel_bfs/problem_utils.h
        include/parallel_bfs/search/node.h
        include/parallel_bfs/search/parallel_bfs.h
        include/parallel_bfs/search/problem.h
        include/parallel_bfs/search/state.h
        include/parallel_bfs/search/sync_bfs.h
        include/parallel_bfs/search/transition_model.h
        include/parallel_bfs/problem_utils/problem_factory.h
        include/parallel_bfs/problem_utils/problem_reader.h
        include/parallel_bfs/problem_utils/problem_writer.h
)

target_link_libraries(parallel_bfs
#        PUBLIC nlohmann_json::nlohmann_json
        PUBLIC yaml-cpp::yaml-cpp
)

target_include_directories(parallel_bfs PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)

set_target_properties(parallel_bfs PROPERTIES LINKER_LANGUAGE CXX)