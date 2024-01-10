################## External libraries ###################
include(FetchContent)
FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz)
FetchContent_MakeAvailable(json)

####################### Sources #########################
add_library(parallel_bfs
        include/parallel_bfs/search.h
        include/parallel_bfs/problem_utils.h
        include/parallel_bfs/search/problem.h
        include/parallel_bfs/search/transition_model.h
        include/parallel_bfs/search/state.h
        include/parallel_bfs/search/sync_bfs.h
        include/parallel_bfs/search/parallel_bfs.h
        include/parallel_bfs/search/node.h
        include/parallel_bfs/problem_utils/problem_factory.h
        include/parallel_bfs/problem_utils/random_builder.h
        include/parallel_bfs/problem_utils/problem_parser.h
        include/parallel_bfs/problem_utils/problem_reader.h
        include/parallel_bfs/problem_utils/problem_writer.h
        src/problem_utils/json_writer.cpp
)

target_link_libraries(parallel_bfs PRIVATE nlohmann_json::nlohmann_json)

target_include_directories(parallel_bfs PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)

set_target_properties(parallel_bfs PROPERTIES LINKER_LANGUAGE CXX)