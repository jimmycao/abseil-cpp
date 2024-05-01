#include <glog/logging.h>

#include "archive_text.h"
#include "archive_binary.h"

#include "line_reader.h"

namespace mlf {

char* LineReader::getline(FILE* f) {
    return this->getdelim(f, '\n');
}

char* LineReader::getdelim(FILE* f, char delim) {
    ssize_t ret = ::getdelim(&_buffer, &_buf_size, delim, f);

    if (ret >= 0) {
        if (ret >= 1 && _buffer[ret - 1] == delim) {
            _buffer[--ret] = 0;
        }

        _length = (size_t)ret;
        return _buffer;
    } else {
        _length = 0;
        PCHECK(feof(f));
        return NULL;
    }
};

char* LineReader::get() {
    return _buffer;
};

template<>
TextArchive LineReader::get<TextArchive>() {
    TextArchive oar;
    if (_buffer == nullptr) {
        return oar;
    }
    oar.set_buffer(_buffer, _length, _length, ::free);
    _buffer = NULL;
    _buf_size = 0;
    _length = 0;
    return oar;
}

template<>
BinaryArchive LineReader::get<BinaryArchive>() {
    BinaryArchive oar;
    if (_buffer == nullptr) {
        return oar;
    }
    oar.set_buffer(_buffer, _length, _length, ::free);
    _buffer = NULL;
    _buf_size = 0;
    _length = 0;
    return oar;
}

size_t LineReader::length() {
    return _length;
};

void LineReader::reset() {
    if (_buffer != NULL) {
        ::free(_buffer);
    }
    _buffer = NULL;
    _buf_size = 0;
    _length = 0;
};

LineReader::~LineReader() {
    if (_buffer != NULL) {
        ::free(_buffer);
    }
}

} // namespace mlf
