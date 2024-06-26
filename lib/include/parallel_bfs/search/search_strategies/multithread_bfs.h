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
#include <future>
#include "bfs.h"
#include "../problem.h"
#include "../node.h"
#include "../state.h"
#include "../transition_model.h"


namespace parallel_bfs::detail {
    class SearchStatusController {
    public:
        void signal_solution_found() {
            ssource_solution_found.request_stop();
            ssource_search_finished.request_stop();
        }

        void signal_search_finished() { ssource_search_finished.request_stop(); }

        [[nodiscard]] bool solution_found() const { return ssource_solution_found.stop_requested(); }

        [[nodiscard]] bool search_finished() const { return ssource_search_finished.stop_requested(); }

        [[nodiscard]] std::stop_token get_solution_token() const { return ssource_solution_found.get_token(); }

        [[nodiscard]] std::stop_token get_search_token() const { return ssource_search_finished.get_token(); }

        std::stop_source ssource_solution_found;
        std::stop_source ssource_search_finished;
    };


    template<Searchable State, std::derived_from<BaseTransitionModel<State>> TM>
    class Worker {
    public:
        std::future<std::shared_ptr<Node<State>>> start_search(std::shared_ptr<Node<State>> init_node, const Problem<State, TM> &problem, SearchStatusController status_controller) {
            frontier.push_back(std::move(init_node));
            status = std::move(status_controller);
            return std::async(std::launch::async, [this, &problem] { return search(problem); });
        }

        /**
         * @brief Adds work to the worker if needed.
         *
         * The method attempts to acquire a lock on the worker's mutex. If successful, it adds the given `node`
         * to the `frontier`, releases the lock, and notifies any waiting threads about the availability of work.
         * If the lock cannot be acquired, it means that the worker is already processing work and no action is taken.
         *
         * @param node The node to add to the frontier.
         * @return true if the work was added, false otherwise.
         */
        bool add_work_if_needed(std::shared_ptr<Node<State>> node) noexcept {
            std::unique_lock lock{mutex, std::defer_lock};
            if (lock.try_lock()) { // If the lock is available, it means that the worker is waiting for work
                frontier.push_back(std::move(node));
                lock.unlock();
                condition.notify_one();
                return true;
            }
            return false;
        }

    private:
        std::shared_ptr<Node<State>> search(const Problem<State, TM> &problem) {
            while (!status.solution_found()) {
                std::unique_lock lock{mutex};
                if (frontier.empty() && status.search_finished()) break;
                condition.wait(lock, status.get_search_token(), [this] { return !frontier.empty(); });
                auto solution = interruptible_bfs(frontier, problem, status.ssource_solution_found);
                if (solution != nullptr) return solution;
                // Release lock with its destructor
            }
            return nullptr;
        }

        std::deque<std::shared_ptr<Node<State>>> frontier{};
        SearchStatusController status;
        mutable std::mutex mutex;
        mutable std::condition_variable_any condition; // We use condition_variable_any to add a stop_token to the wait
    };



    template<Searchable State, std::derived_from<BaseTransitionModel<State>> TM>
    class ThreadDirector {
    public:
        explicit ThreadDirector(unsigned int num_threads, unsigned int min_starting_points)
                : num_threads{num_threads}, min_starting_points{min_starting_points}, workers(num_threads) {}

        [[nodiscard]] std::shared_ptr<Node<State>> search(const Problem<State, TM> &problem) {
            // First create enough initial work
            main_frontier.push_back(std::make_shared<Node<State>>(problem.initial()));
            generate_work(problem, min_starting_points);
            if (status.solution_found() || main_frontier.empty()) return solution;

            // Start the workers
            auto futures = start_workers(problem);

            // While waiting for a solution, keep generating and distributing work
            while (!status.solution_found() && !main_frontier.empty()) {
                generate_work(problem, 1);
                distribute_work();
            }

            // Signal that main has finished searching
            status.signal_search_finished();

            // Ensure that all threads have finished to avoid data races
            for (auto &future: futures)
                if (auto result = future.get(); result != nullptr) solution = result;
            return solution;
        }

        void generate_work(const Problem<State, TM> &problem, unsigned int new_childs_count) {
            std::size_t limit = main_frontier.size() + new_childs_count;
            auto possible_solution = detail::bfs_with_limit(main_frontier, problem, limit);
            if (possible_solution != nullptr) {
                status.signal_solution_found();
                solution = possible_solution;
            }
        }

        void distribute_work() {
            for (unsigned int i = 0; i < num_threads; ++i) {
                if (main_frontier.empty() || status.solution_found()) break;
                if (workers[i].add_work_if_needed(main_frontier.front())) main_frontier.pop_front();
            }
        }

        std::vector<std::future<std::shared_ptr<Node<State>>>> start_workers(const Problem<State, TM> &problem) {
            std::vector<std::future<std::shared_ptr<Node<State>>>> futures(num_threads);
            for (unsigned int i = 0; i < num_threads; ++i) {
                futures[i] = workers[i].start_search(main_frontier.front(), problem, status);
                main_frontier.pop_front();
            }
            return futures;
        }


    private:
        const unsigned int num_threads;
        const unsigned int min_starting_points;
        std::vector<Worker<State, TM>> workers;
        std::deque<std::shared_ptr<Node<State>>> main_frontier{};
        SearchStatusController status;
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
