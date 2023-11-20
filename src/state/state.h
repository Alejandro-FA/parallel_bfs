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
    return lhs.is_equal(rhs);
}

template<> struct std::hash<State> {
    std::size_t operator()(const State &s) const noexcept { return s.hash(); }
};

#endif //PARALLEL_BFS_STATE_H
