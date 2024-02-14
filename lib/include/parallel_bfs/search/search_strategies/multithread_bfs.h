//
// Created by Alejandro Fernández on 06/11/2023.
//

#ifndef PARALLEL_BFS_MULTITHREAD_BFS_H
#define PARALLEL_BFS_MULTITHREAD_BFS_H

#include <vector>
#include <deque>
#include <memory>
#include <stop_token>
#include <mutex>
#include <condition_variable>
#include <iostream>
#include "bfs.h"


namespace parallel_bfs::detail {
    template<Searchable State, std::derived_from<BaseTransitionModel<State>> TM>
    class Worker {
    public:
        explicit Worker(
                std::deque<std::shared_ptr<Node<State>>> &frontier,
                const Problem<State, TM> &problem,
                std::mutex &mutex,
                std::condition_variable_any &condition,
                std::stop_source ssource_solution_found,
                std::stop_source ssource_search_finished
        ) : frontier{frontier}, problem{problem}, mutex{mutex}, condition{condition},
        ssource_solution_found{std::move(ssource_solution_found)}, ssource_search_finished{std::move(ssource_search_finished)} {}

        std::shared_ptr<Node<State>> operator()() {
            while (!ssource_solution_found.stop_requested()) {
                std::unique_lock lock{mutex};
                if (frontier.empty() && ssource_search_finished.stop_requested()) break;
                condition.wait(lock, ssource_search_finished.get_token(), [this] { return !frontier.empty(); });

                while (!frontier.empty() && !ssource_solution_found.stop_requested()) {
                    auto node = frontier.front();
                    if (problem.is_goal(node->state())) {
                        ssource_solution_found.request_stop();
                        return node;
                    }
                    frontier.pop_front();
                    for (const auto &child: problem.expand(node)) frontier.push_back(child);
                }
                // Release lock with its destructor
            }

            return nullptr;
        }

    private:
        std::deque<std::shared_ptr<Node<State>>> &frontier;
        const Problem<State, TM> &problem;
        std::mutex &mutex;
        std::condition_variable_any &condition;
        std::stop_source ssource_solution_found{};
        std::stop_source ssource_search_finished{};
    };



    template<Searchable State, std::derived_from<BaseTransitionModel<State>> TM>
    class ThreadDirector {
    public:
        explicit ThreadDirector(unsigned int num_threads, unsigned int min_starting_points)
                : num_threads{num_threads}, min_starting_points{min_starting_points}, mutexes(num_threads),
                  conditions(num_threads), frontiers(num_threads) {}

        [[nodiscard]] std::shared_ptr<Node<State>> search(const Problem<State, TM> &problem) {
            // First create enough initial work
            main_frontier.push_back(std::make_shared<Node<State>>(problem.initial()));
            generate_work(problem, min_starting_points);
            if (ssource_solution_found.stop_requested() || main_frontier.empty()) return solution;

            // Then, initialize the workers
            auto futures = init_workers(problem);

            // While waiting for a solution, keep generating and distributing work
            while (!ssource_solution_found.stop_requested() && !main_frontier.empty()) {
                generate_work(problem, 1);
                distribute_work();
            }

            // Signal that main has finished searching
            ssource_search_finished.request_stop();

            // Ensure that all threads have finished to avoid data races
            for (auto &future: futures)
                if (auto result = future.get(); result != nullptr) solution = result;
            return solution;
        }

        void generate_work(const Problem<State, TM> &problem, unsigned int new_childs_count) {
            std::size_t limit = main_frontier.size() + new_childs_count;
            auto possible_solution = detail::bfs_with_limit(main_frontier, problem, limit);
            if (possible_solution != nullptr) {
                ssource_solution_found.request_stop();
                solution = possible_solution;
            }
        }

        void distribute_work() {
            for (unsigned int i = 0; i < num_threads; ++i) {
                if (main_frontier.empty() || ssource_solution_found.stop_requested()) break;
                std::unique_lock lock{mutexes[i], std::defer_lock};
                if (lock.try_lock()) { // If the lock is available, it means that the worker is waiting for work
                    frontiers[i].push_back(main_frontier.front());
                    main_frontier.pop_front();
                    lock.unlock();
                    conditions[i].notify_one();
                }
            }
        }

        std::vector<std::future<std::shared_ptr<Node<State>>>> init_workers(const Problem<State, TM> &problem) {
            distribute_work();
            std::vector<std::future<std::shared_ptr<Node<State>>>> futures(num_threads);
            for (unsigned int i = 0; i < num_threads; ++i) {
                futures[i] = std::async(std::launch::async, Worker<State, TM>{
                        frontiers[i], problem, mutexes[i], conditions[i], ssource_solution_found, ssource_search_finished
                });
            }
            return futures;
        }

    private:
        const unsigned int num_threads;
        const unsigned int min_starting_points;
        std::vector<std::mutex> mutexes;
        std::vector<std::condition_variable_any> conditions;
        std::vector<std::deque<std::shared_ptr<Node<State>>>> frontiers;
        std::deque<std::shared_ptr<Node<State>>> main_frontier{};
        std::stop_source ssource_solution_found{};
        std::stop_source ssource_search_finished{};
        std::shared_ptr<Node<State>> solution{nullptr};
    };
}


namespace parallel_bfs {
    /// In order to avoid data races, ParallelBFSTasks only works with tree-like search.
    template<Searchable State, std::derived_from<BaseTransitionModel<State>> TM>
    [[nodiscard]] std::shared_ptr<Node<State>> multithread_bfs(const Problem<State, TM> &problem) {
        detail::ThreadDirector<State, TM> director{std::thread::hardware_concurrency(), std::thread::hardware_concurrency() * 4};
        return director.search(problem);
    }
}
#endif //PARALLEL_BFS_MULTITHREAD_BFS_H
