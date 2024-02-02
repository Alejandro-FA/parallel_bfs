//
// Created by Alejandro Fernández on 06/11/2023.
//

#ifndef PARALLEL_BFS_SYNC_BFS_H
#define PARALLEL_BFS_SYNC_BFS_H

#include <queue>


namespace parallel_bfs {
    template<Searchable State, std::derived_from<BaseTransitionModel<State>> TM, SearchType type = SearchType::tree_like>
    [[nodiscard]] std::shared_ptr<Node<State>> sync_bfs(const Problem<State, TM> &problem) {
        return detail::bfs<State, TM, type>(std::make_shared<Node<State>>(problem.initial()), problem);
    }
}

#endif //PARALLEL_BFS_SYNC_BFS_H
