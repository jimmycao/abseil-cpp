#pragma once 

#include <functional>

namespace mlf {

template<class T>
class Hash {
public:
    static std::size_t hash(T &t) {
        static Hash<T> singleton;
        return singleton._hash(t);
    }

private:
    decltype(std::hash<T>{}) _hash = std::hash<T>{};
};

// used by rehash
inline uint64_t murmur_hash(uint64_t x) {
    // copy from MurmurHash3
    x ^= x >> 33;
    x *= 0xff51afd7ed558ccd;
    x ^= x >> 33;
    x *= 0xc4ceb9fe1a85ec53;
    x ^= x >> 33;
    return x;
};

// used by rehash
inline uint64_t murmur_hash_half(uint64_t x) {
    x ^= x >> 33;
    x *= 0xff51afd7ed558ccd;
    x ^= x >> 33;
    return x;
};

} // namespace mlf
