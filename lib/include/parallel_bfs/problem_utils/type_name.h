//
// Created by alejandro on 27/01/24.
//

#ifndef PARALLEL_BFS_TYPE_NAME_H
#define PARALLEL_BFS_TYPE_NAME_H

// This code is not original and was taken from:
// https://stackoverflow.com/questions/81870/is-it-possible-to-print-a-variables-type-in-standard-c/64490578#64490578

#include <string_view>
#include <source_location>

namespace parallel_bfs {
    template <typename T> constexpr std::string_view type_name();

    template <>
    constexpr std::string_view type_name<void>() { return "void"; }
}

namespace detail {
    using type_name_prober = void;

    template <typename T>
    constexpr std::string_view wrapped_type_name() {
        return std::source_location::current().function_name();
    }

    constexpr std::size_t wrapped_type_name_prefix_length() {
        return wrapped_type_name<type_name_prober>()
            .find(parallel_bfs::type_name<type_name_prober>());
    }

    constexpr std::size_t wrapped_type_name_suffix_length() {
        return wrapped_type_name<type_name_prober>().length()
            - wrapped_type_name_prefix_length()
            - parallel_bfs::type_name<type_name_prober>().length();
    }
}

namespace parallel_bfs {
    template <typename T>
    constexpr std::string_view type_name() {
        constexpr auto wrapped_name = detail::wrapped_type_name<T>();
        constexpr auto prefix_length = detail::wrapped_type_name_prefix_length();
        constexpr auto suffix_length = detail::wrapped_type_name_suffix_length();
        constexpr auto type_name_length = wrapped_name.length() - prefix_length - suffix_length;
        return wrapped_name.substr(prefix_length, type_name_length);
    }
}

#endif //PARALLEL_BFS_TYPE_NAME_H
