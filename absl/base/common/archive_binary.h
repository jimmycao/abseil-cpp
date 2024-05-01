#pragma once

#include <cstring>

#include "common/archive_base.h"
#include "common/archive_template.h"

namespace mlf {

class BinaryType {
};

typedef Archive<BinaryType> BinaryArchive;

template<>
class Archive<BinaryType> : public ArchiveBase {
public:
    #define MICRO_REPEAT_PATTERN(T) \
    BinaryArchive& operator>>(T& x) { \
        _get_raw(x); \
        return *this; \
    } \
    BinaryArchive& operator<<(const T& x) { \
        _put_raw(x); \
        return *this; \
    }
    MICRO_REPEAT_PATTERN(int16_t)
    MICRO_REPEAT_PATTERN(uint16_t)
    MICRO_REPEAT_PATTERN(int32_t)
    MICRO_REPEAT_PATTERN(uint32_t)
    MICRO_REPEAT_PATTERN(int64_t)
    MICRO_REPEAT_PATTERN(uint64_t)
    MICRO_REPEAT_PATTERN(float)
    MICRO_REPEAT_PATTERN(double)
    MICRO_REPEAT_PATTERN(signed char)
    MICRO_REPEAT_PATTERN(unsigned char)
    MICRO_REPEAT_PATTERN(bool)
    #undef MICRO_REPEAT_PATTERN

    template<class T>
    T get() {
        T x;
        *this >> x;
        return x;
    }

private:
    template<class T>
    void _get_raw(T& x) {
        prepare_read(sizeof(T));
        memcpy(&x, _cursor, sizeof(T));
        advance_cursor(sizeof(T));
    }

    template<class T>
    T _get_raw() {
        T x;
        _get_raw<T>(x);
        return x;
    }

    template<class T>
    void _put_raw(const T& x) {
        prepare_write(sizeof(T));
        memcpy(_finish, &x, sizeof(T));
        advance_finish(sizeof(T));
    }
};

inline BinaryArchive& operator<<(BinaryArchive& ar, const std::string& s) {
    ar << (size_t)s.length();
    ar.write(&s[0], s.length());
    return ar;
}

inline BinaryArchive& operator>>(BinaryArchive& ar, std::string& s) {
    size_t len = ar.template get<size_t>();
    ar.prepare_read(len);
    s.assign(ar.cursor(), len);
    ar.advance_cursor(len);
    return ar;
}

} // namespace mlf
