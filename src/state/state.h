//
// Created by Alejandro Fernández on 19/11/2023.
//

#ifndef PARALLEL_BFS_STATE_H
#define PARALLEL_BFS_STATE_H

class State {
public:
    friend std::ostream &operator<<(std::ostream &os, const State &s);

    friend bool operator==(const State &lhs, const State &rhs);

    friend std::hash<State>;

protected:
    [[nodiscard]] virtual std::ostream &print(std::ostream &os) const = 0;

    [[nodiscard]] virtual bool is_equal(const State &rhs) const = 0;

    [[nodiscard]] virtual std::size_t hash() const = 0;
};

std::ostream &operator<<(std::ostream &os, const State &s) { return s.print(os); }

bool operator==(const State &lhs, const State &rhs) {
    return typeid(lhs) == typeid(rhs) && lhs.is_equal(rhs);
}

bool operator!=(const State &lhs, const State &rhs) { return !(lhs == rhs); }

template<>
struct std::hash<State> {
    std::size_t operator()(const State &s) const noexcept { return s.hash(); }
};

template<std::derived_from<State> T>
struct std::hash<T> {
    std::size_t operator()(const T &s) const noexcept { return std::hash<State>{}(s); }
};

#endif //PARALLEL_BFS_STATE_H
