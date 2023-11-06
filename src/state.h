//
// Created by Alejandro Fernández on 04/11/2023.
//

#ifndef PARALLEL_BFS_STATE_H
#define PARALLEL_BFS_STATE_H

typedef uint32_t state_space_size_t;

class State {
public:
    explicit State(state_space_size_t value) : _value{value} {}

    friend bool operator==(const State &lhs, const State &rhs);

    friend std::ostream &operator<<(std::ostream &, const State &);

    friend std::hash<State>;

private:
    const state_space_size_t _value;
};

bool operator==(const State &lhs, const State &rhs) { return lhs._value == rhs._value; }

std::ostream &operator<<(std::ostream &os, const State &s) { return os << s._value; }

template<> struct std::hash<State> {
    std::size_t operator()(const State &s) const noexcept { return std::hash<state_space_size_t>{}(s._value); }
};

#endif //PARALLEL_BFS_STATE_H
