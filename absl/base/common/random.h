#pragma once

#include <random>

namespace mlf {

class Random {
public:
    static Random& instance() {
        static Random singleton;
        return singleton;
    }

    Random();

    template<class T>
    T uniform(T start, T end);

    template<class T>
    T normal(T start, T end);

    template<class T>
    std::vector<T> uniform(T start, T end, int num);

    template<class T>
    std::vector<T> normal(T start, T end, int num);

private:
    std::default_random_engine _engine;
};

} // namespace mlf
