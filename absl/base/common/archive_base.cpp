#include <string>
#include <cstring>

#include "log.h"
#include "likely.h"

#include "archive_base.h"

namespace mlf {

ArchiveBase::ArchiveBase(ArchiveBase&& other) :
    _buffer(other._buffer),
    _cursor(other._cursor),
    _finish(other._finish),
    _limit(other._limit),
    _delete_handler(std::move(other._delete_handler)) {
    other._buffer = NULL;
    other._cursor = NULL;
    other._finish = NULL;
    other._limit = NULL;
    other._delete_handler = nullptr;
}

ArchiveBase& ArchiveBase::operator=(ArchiveBase&& other) {
    if (this != &other) {
        _free_buffer();
        _buffer = other._buffer;
        _cursor = other._cursor;
        _finish = other._finish;
        _limit = other._limit;
        _delete_handler = std::move(other._delete_handler);
        other._buffer = NULL;
        other._cursor = NULL;
        other._finish = NULL;
        other._limit = NULL;
        other._delete_handler = nullptr;
    }
    return *this;
}

ArchiveBase::~ArchiveBase() {
    _free_buffer();
}

char* ArchiveBase::buffer() {
    return _buffer;
}

void ArchiveBase::set_read_buffer(char* buffer, size_t length, 
                                  std::function<void(char*)>&& delete_handler) {
    size_t capacity = length;
    set_buffer(buffer, length, capacity, std::move(delete_handler));
}

void ArchiveBase::set_write_buffer(char* buffer, size_t capacity, 
                                   std::function<void(char*)>&& delete_handler) {
    size_t length = 0;
    set_buffer(buffer, length, capacity, std::move(delete_handler));
}

void ArchiveBase::set_buffer(char* buffer, size_t length, size_t capacity, 
                             std::function<void(char*)>&& delete_handler) {
    MCHECK(length <= capacity);
    _free_buffer();
    _buffer = buffer;
    _cursor = _buffer;
    _finish = buffer + length;
    _limit = buffer + capacity;
    _delete_handler = std::move(delete_handler);
}

char* ArchiveBase::cursor() {
    return _cursor;
}

void ArchiveBase::set_cursor(char* cursor) {
    MCHECK(cursor >= _buffer && cursor <= _finish);
    _cursor = cursor;
}

void ArchiveBase::advance_cursor(size_t offset) {
    MCHECK(offset <= size_t(_finish - _cursor));
    _cursor += offset;
}

char* ArchiveBase::finish() {
    return _finish;
}

void ArchiveBase::set_finish(char* finish) {
    MCHECK(finish >= _cursor && finish <= _limit);
    _finish = finish;
}

void ArchiveBase::advance_finish(size_t offset) {
    MCHECK(offset <= size_t(_limit - _finish));
    _finish += offset;
}

char* ArchiveBase::limit() {
    return _limit;
}

size_t ArchiveBase::position() {
    return _cursor - _buffer;
}

size_t ArchiveBase::length() {
    return _finish - _buffer;
}

size_t ArchiveBase::capacity() {
    return _limit - _buffer;
}

bool ArchiveBase::empty() {
    return _finish == _buffer;
}

void ArchiveBase::reset() {
    _free_buffer();
    _buffer = NULL;
    _cursor = NULL;
    _finish = NULL;
    _limit = NULL;
}

void ArchiveBase::clear() {
    _cursor = _buffer;
    _finish = _buffer;
}

char* ArchiveBase::release() {
    char* buf = _buffer;
    _buffer = NULL;
    _cursor = NULL;
    _finish = NULL;
    _delete_handler = nullptr;
    return buf;
}

void ArchiveBase::resize(size_t newsize) {
    if (unlikely(newsize > capacity())) {
        reserve(std::max(capacity() * 2, newsize));
    }
    _finish = _buffer + newsize;
    _cursor = std::min(_cursor, _finish);
}

void ArchiveBase::reserve(size_t newcap) {
    if (newcap > capacity()) {
        char* newbuf = nullptr;
        newbuf = new char[newcap];
        if (length() > 0) {
            memcpy(newbuf, _buffer, length());
        }
        _cursor = newbuf + (_cursor - _buffer);
        _finish = newbuf + (_finish - _buffer);
        _limit = newbuf + newcap;
        _free_buffer();
        _buffer = newbuf;
        _delete_handler = std::default_delete<char[]>();
    }
}

void ArchiveBase::prepare_read(size_t size) {
    if (unlikely(!(size <= size_t(_finish - _cursor)))) {
        MCHECK(size <= size_t(_finish - _cursor));
    }
}

void ArchiveBase::prepare_write(size_t size) {
    if (unlikely(size > size_t(_limit - _finish))) {
        reserve(std::max(capacity() * 2, length() + size));
    }
}

void ArchiveBase::read(void* data, size_t size) {
    if (size > 0) {
        prepare_read(size);
        memcpy(data, _cursor, size);
        advance_cursor(size);
    }
}

void ArchiveBase::read_back(void* data, size_t size) {
    if (size > 0) {
        MCHECK(size <= size_t(_finish - _cursor));
        memcpy(data, _finish - size, size);
        _finish -= size;
    }
}

void ArchiveBase::write(const void* data, size_t size) {
    if (size > 0) {
        prepare_write(size);
        memcpy(_finish, data, size);
        advance_finish(size);
    }
}

void ArchiveBase::_free_buffer() {
    if (_delete_handler) {
        _delete_handler(_buffer);
    }
    _delete_handler = nullptr;
}

} // namespace mlf
