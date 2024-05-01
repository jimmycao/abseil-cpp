#pragma once

#include <stdio.h>

#include "common/log.h"
#include "common/archive_base.h"
#include "common/archive_template.h"

namespace mlf {

class TextArchiveType {
};
typedef Archive<TextArchiveType> TextArchive;

template<>
class Archive<TextArchiveType> : public ArchiveBase {
public:
    #define MICRO_REPEAT_PATTERN(T) \
    TextArchive& operator>>(T& x) { \
        _get_arithmetic(x); \
        return *this; \
    } \
    TextArchive& operator<<(const T& x) { \
        _put_arithmetic(x); \
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

    #define MIO_REPEAT_PATTERN(T, format) \
    void _get_arithmetic(T& x) { \
        char* next = cursor(); \
        while (next < finish() && *next != '\t') { \
            next++; \
        } \
        prepare_read(next - cursor()); \
        sscanf(cursor(), format, &x); \
        advance_cursor(next - cursor() + 1); \
    }
    MIO_REPEAT_PATTERN(int16_t, "%hd")
    MIO_REPEAT_PATTERN(uint16_t, "%hu")
    MIO_REPEAT_PATTERN(int32_t, "%d")
    MIO_REPEAT_PATTERN(uint32_t, "%u")
    MIO_REPEAT_PATTERN(int64_t, "%ld")
    MIO_REPEAT_PATTERN(uint64_t, "%lu")
    MIO_REPEAT_PATTERN(float, "%f")
    MIO_REPEAT_PATTERN(double, "%lf")
    MIO_REPEAT_PATTERN(signed char, "%c")
    MIO_REPEAT_PATTERN(unsigned char, "%c")
    #undef MIO_REPEAT_PATTERN

    #define MIO_REPEAT_PATTERN(T, format) \
    void _put_arithmetic(const T& x) { \
        prepare_write(32); \
        int offset = snprintf(_finish, 32, format, x); \
        char* i = finish() + offset; \
        *i = '\t'; \
        advance_finish(offset + 1); \
    }
    MIO_REPEAT_PATTERN(int16_t, "%hd")
    MIO_REPEAT_PATTERN(uint16_t, "%hu")
    MIO_REPEAT_PATTERN(int32_t, "%d")
    MIO_REPEAT_PATTERN(uint32_t, "%u")
    MIO_REPEAT_PATTERN(int64_t, "%ld")
    MIO_REPEAT_PATTERN(uint64_t, "%lu")
    MIO_REPEAT_PATTERN(float, "%f")
    MIO_REPEAT_PATTERN(double, "%lf")
    MIO_REPEAT_PATTERN(signed char, "%c")
    MIO_REPEAT_PATTERN(unsigned char, "%c")
    #undef MIO_REPEAT_PATTERN

    void _get_arithmetic(bool& x) {
        int y;
        _get_arithmetic(y);
        MCHECK(y == 0 || y == 1);
        x = y;
    }

    void _put_arithmetic(bool x) {
        _put_arithmetic((int)x);
    }
};

inline TextArchive& operator<<(TextArchive& ar, const std::string& s) {
    ar.write(&s[0], s.length() + 1);
    return ar;
}

inline TextArchive& operator>>(TextArchive& ar, std::string& s) {
    char* next = ar.cursor();
    while (next < ar.finish() && *next != '\0') {
        next++;
    }
    s.assign(ar.cursor(), next - ar.cursor());
    ar.set_cursor(std::min(++next, ar.finish()));
    return ar;
}

} // namespace mlf
