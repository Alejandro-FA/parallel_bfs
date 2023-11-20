//
// Created by Alejandro Fernández on 19/11/2023.
//

#ifndef PARALLEL_BFS_GRAPH_STATE_H
#define PARALLEL_BFS_GRAPH_STATE_H

typedef uint32_t value_t;

class GraphState final : public State {
public:
    explicit GraphState(value_t value) : _value{value} {}

protected:
    const value_t _value;

    [[nodiscard]] std::ostream &print(std::ostream &os) const override { return os << _value; }

    [[nodiscard]] bool is_equal(const State &rhs) const override {
        if (const auto* p = dynamic_cast<const GraphState*>(&rhs)) return _value == p->_value;
        return false;
    }

    [[nodiscard]] std::size_t hash() const override { return std::hash<value_t>{}(_value); }
};

#endif //PARALLEL_BFS_GRAPH_STATE_H
