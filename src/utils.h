//
// Created by alejandro on 30/01/24.
//

#ifndef PARALLEL_BFS_PROJECT_UTILS_H
#define PARALLEL_BFS_PROJECT_UTILS_H

#include <filesystem>
#include <functional>
#include <cmath>


struct ExecutionTime {
    using TimePoint = std::chrono::time_point<std::chrono::high_resolution_clock>;
    explicit ExecutionTime(TimePoint start, TimePoint end) : start{start}, end{end} {}

    [[nodiscard]] double as_milliseconds() const {
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
        return std::round(duration / 10) / 100;
    }

    [[nodiscard]] double as_seconds() const {
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        return std::round(duration / 10) / 100;
    }

    static TimePoint now() { return std::chrono::high_resolution_clock::now(); }

    TimePoint start;
    TimePoint end;
};


template<typename OutputType>
struct TimedResult {
    OutputType output;
    ExecutionTime elapsed_time;

    explicit TimedResult(OutputType&& output, ExecutionTime time) : output{std::move(output)}, elapsed_time{time} {}
};


template<typename F, typename... Args> requires std::invocable<F, Args...> && (!std::is_void_v<std::invoke_result_t<F, Args...>>)
TimedResult<std::invoke_result_t<F, Args...>> invoke_and_time(F&& f, Args&&... args) {
    auto start = ExecutionTime::now();
    auto output {std::invoke<F, Args...>(std::forward<F>(f), std::forward<Args>(args)...)};
    auto end = ExecutionTime::now();
    return TimedResult{std::move(output), ExecutionTime{start, end}};
}


template<typename F, typename... Args> requires std::invocable<F, Args...>
ExecutionTime invoke_and_time(F&& f, Args&&... args) {
    auto start = ExecutionTime::now();
    std::invoke<F, Args...>(std::forward<F>(f), std::forward<Args>(args)...);
    auto end = ExecutionTime::now();
    return ExecutionTime{start, end};
}


#endif //PARALLEL_BFS_PROJECT_UTILS_H
