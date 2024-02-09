//
// Created by alejandro on 08/02/24.
//

#ifndef PARALLEL_BFS_PROJECT_STATISTICS_H
#define PARALLEL_BFS_PROJECT_STATISTICS_H

#include <vector>
#include <string>
#include <execution>
#include <numeric>
#include <algorithm>
#include <cmath>


class Statistic {
public:
    virtual ~Statistic() = default;
    [[nodiscard]] virtual double compute(const std::vector<double>& data) const = 0;
    [[nodiscard]] virtual std::string name() const = 0;

    std::execution::unsequenced_policy policy = std::execution::unseq;
};


class Average : public Statistic {
public:
    [[nodiscard]] double compute(const std::vector<double>& data) const override {
        if (data.empty()) throw std::invalid_argument("Data cannot be empty");
        double sum = std::reduce(policy, data.begin(), data.end(), 0.0);
        return sum / static_cast<double>(data.size());
    }

    [[nodiscard]] inline std::string name() const override { return "Average"; }
};


class Median : public Statistic {
public:
    [[nodiscard]] double compute(const std::vector<double>& data) const override {
        if (data.empty()) throw std::invalid_argument("Data cannot be empty");

        std::vector<double> mutable_data = data;
        long n = static_cast<long>(mutable_data.size()) / 2;
        std::nth_element(policy, mutable_data.begin(), mutable_data.begin() + n, mutable_data.end());

        auto median = mutable_data[n];
        if(!(mutable_data.size() & 1)) { // If the set size is even
            std::nth_element(policy, mutable_data.begin(), mutable_data.begin() + n - 1, mutable_data.end());
            median = (median + mutable_data[n-1]) / 2.0;
        }
        return median;
    }

    [[nodiscard]] inline std::string name() const override { return "Median"; }
};


class StandardDeviation : public Statistic {
public:
    [[nodiscard]] double compute(const std::vector<double>& data) const override {
        if (data.empty()) throw std::invalid_argument("Data cannot be empty");
        double mean = Average{}.compute(data);
        double sum = std::transform_reduce(
            policy,
            data.begin(), data.end(),
            0.0,
            std::plus<>(),
            [mean](double x) { return (x - mean) * (x - mean); }
        );
        return std::sqrt(sum / static_cast<double>(data.size()));
    }

    [[nodiscard]] inline std::string name() const override { return "Standard Deviation"; }
};


#endif //PARALLEL_BFS_PROJECT_STATISTICS_H
