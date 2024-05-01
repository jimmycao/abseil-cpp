#include "random.h"

#define SPECIALIZE(INTERFACE, TYPE, FUNC) \
template<> \
TYPE INTERFACE<TYPE>(TYPE start, TYPE end) { \
    return FUNC<TYPE>(start, end)(_engine); \
}; \
template<> \
std::vector<TYPE> INTERFACE<TYPE>(TYPE start, TYPE end, int num) { \
    std::vector<TYPE> result; \
    for (int i = 0; i < num; i++) \
        result.emplace_back(FUNC<TYPE>(start, end)(_engine)); \
    return result; \
};

namespace mlf {

Random::Random() {
    std::random_device rd;
    std::seed_seq sseq {rd(), rd(), rd()};
    _engine.seed(sseq);
}

SPECIALIZE(Random::uniform, float, std::uniform_real_distribution)
SPECIALIZE(Random::uniform, double, std::uniform_real_distribution)
SPECIALIZE(Random::uniform, long double, std::uniform_real_distribution)

SPECIALIZE(Random::uniform, short, std::uniform_int_distribution)
SPECIALIZE(Random::uniform, int, std::uniform_int_distribution)
SPECIALIZE(Random::uniform, long, std::uniform_int_distribution)
SPECIALIZE(Random::uniform, long long, std::uniform_int_distribution)

SPECIALIZE(Random::uniform, unsigned short, std::uniform_int_distribution)
SPECIALIZE(Random::uniform, unsigned int, std::uniform_int_distribution)
SPECIALIZE(Random::uniform, unsigned long, std::uniform_int_distribution)
SPECIALIZE(Random::uniform, unsigned long long, std::uniform_int_distribution)

SPECIALIZE(Random::normal, float, std::normal_distribution)
SPECIALIZE(Random::normal, double, std::normal_distribution)
SPECIALIZE(Random::normal, long double, std::normal_distribution)

} // namespace mlf

#undef SPECIALIZE
