#pragma once

#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>

namespace mlf {

class LineReader {
public:
    LineReader() = default;
    LineReader(const LineReader&) = delete;
    LineReader& operator=(const LineReader&) = delete;
    ~LineReader();
    
    char* getline(FILE* f);
    char* getdelim(FILE* f, char delim);
    char* get();

    template<class ARCHIVE>
    ARCHIVE get();

    size_t length();
    void reset();

private:
    char* _buffer = NULL;
    size_t _buf_size = 0;
    size_t _length = 0;
};

} // namespace mlf
