#pragma once

#include <map>
#include <set>
#include <vector>
#include <valarray>
#include <unordered_map>
#include <unordered_set>

namespace mlf {

template<class Type>
class Archive {
};

//to array
template<class AR, class T, size_t N>
Archive<AR>& operator>>(Archive<AR>& ar, T (&p)[N]) {
    for (size_t i = 0; i < N; i++) {
        ar >> p[i];
    }
    return ar;
}

//from/to vector
template<class AR, class T>
Archive<AR>& operator<<(Archive<AR>& ar, const std::vector<T>& p) {
    ar << (size_t)p.size();
    for (const auto& x : p) {
        ar << x;
    }
    return ar;
}

template<class AR, class T>
Archive<AR>& operator>>(Archive<AR>& ar, std::vector<T>& p) {
    p.resize(ar.template get<size_t>());
    for (auto& x : p) {
        ar >> x;
    }
    return ar;
}

//from/to valarray
template<class AR, class T>
Archive<AR>& operator<<(Archive<AR>& ar, const std::valarray<T>& p) {
    ar << (size_t)p.size();
    for (const auto& x : p) {
        ar << x;
    }
    return ar;
}

template<class AR, class T>
Archive<AR>& operator>>(Archive<AR>& ar, std::valarray<T>& p) {
    p.resize(ar.template get<size_t>());
    for (auto& x : p) {
        ar >> x;
    }
    return ar;
}

//pair
template<class AR, class T1, class T2>
Archive<AR>& operator<<(Archive<AR>& ar, const std::pair<T1, T2>& x) {
    return ar << x.first << x.second;
}

template<class AR, class T1, class T2>
Archive<AR>& operator>>(Archive<AR>& ar, std::pair<T1, T2>& x) {
    return ar >> x.first >> x.second;
}

//<< tuple
template<class AR, class... T>
Archive<AR>& serialize_tuple(Archive<AR>& ar, const std::tuple<T...>& x,
                             std::integral_constant<size_t, 0> n) {
    return ar;
}

template<class AR, class... T, size_t N>
Archive<AR>& serialize_tuple(Archive<AR>& ar, const std::tuple<T...>& x,
                             std::integral_constant<size_t, N> n) {
    return serialize_tuple(ar, x, std::integral_constant<size_t, N - 1>()) << std::get<N - 1>(x);
}

template<class AR, class... T>
Archive<AR>& operator<<(Archive<AR>& ar, const std::tuple<T...>& x) {
    const size_t size = std::tuple_size<std::tuple<T...>>::value;
    return serialize_tuple(ar, x, std::integral_constant<size_t, size>());
}

//>> tuple
template<class AR, class... T>
Archive<AR>& deserialize_tuple(Archive<AR>& ar, std::tuple<T...>& x,
                               std::integral_constant<size_t, 0> n) {
    return ar;
}

template<class AR, class... T, size_t N>
Archive<AR>& deserialize_tuple(Archive<AR>& ar, std::tuple<T...>& x,
                               std::integral_constant<size_t, N> n) {
    return deserialize_tuple(ar, x, std::integral_constant<size_t, N - 1>()) >> std::get<N - 1>(x);
}

template<class AR, class... T>
Archive<AR>& operator>>(Archive<AR>& ar, std::tuple<T...>& x) {
    const size_t size = std::tuple_size<std::tuple<T...>>::value;
    return deserialize_tuple(ar, x, std::integral_constant<size_t, size>());
}

//map
#define MICRO_REPEAT_PATTERN(MAP_TYPE, RESERVE_STATEMENT) \
    template<class AR, class KEY, class VALUE, class... ARGS> \
    Archive<AR>& operator<<(Archive<AR>& ar, const MAP_TYPE<KEY, VALUE, ARGS...>& p) { \
        ar << (size_t)p.size(); \
        for (auto it = p.begin(); it != p.end(); ++it) { \
            ar << *it; \
        } \
        return ar; \
    } \
    template<class AR, class KEY, class VALUE, class... ARGS> \
    Archive<AR>& operator>>(Archive<AR>& ar, MAP_TYPE<KEY, VALUE, ARGS...>& p) { \
        size_t size = ar.template get<size_t>(); \
        p.clear(); \
        RESERVE_STATEMENT; \
        for (size_t i = 0; i < size; i++) { \
            p.insert(ar.template get<std::pair<KEY, VALUE>>()); \
        } \
        return ar; \
    }

MICRO_REPEAT_PATTERN(std::map, )
MICRO_REPEAT_PATTERN(std::multimap, )
MICRO_REPEAT_PATTERN(std::unordered_map, p.reserve(size))
MICRO_REPEAT_PATTERN(std::unordered_multimap, p.reserve(size))

#undef MICRO_REPEAT_PATTERN

//set 
#define MICRO_REPEAT_PATTERN(SET_TYPE, RESERVE_STATEMENT) \
    template<class AR, class KEY, class... ARGS> \
    Archive<AR>& operator<<(Archive<AR>& ar, const SET_TYPE<KEY, ARGS...>& p) { \
        ar << (size_t)p.size(); \
        for (auto it = p.begin(); it != p.end(); ++it) { \
            ar << *it; \
        } \
        return ar; \
    } \
    template<class AR, class KEY, class... ARGS> \
    Archive<AR>& operator>>(Archive<AR>& ar, SET_TYPE<KEY, ARGS...>& p) { \
        size_t size = ar.template get<size_t>(); \
        p.clear(); \
        RESERVE_STATEMENT; \
        for (size_t i = 0; i < size; i++) { \
            p.insert(ar.template get<KEY>()); \
        } \
        return ar; \
    }

MICRO_REPEAT_PATTERN(std::set, )
MICRO_REPEAT_PATTERN(std::multiset, )
MICRO_REPEAT_PATTERN(std::unordered_set, p.reserve(size))
MICRO_REPEAT_PATTERN(std::unordered_multiset, p.reserve(size))

#undef MICRO_REPEAT_PATTERN

} // namespace mlf
