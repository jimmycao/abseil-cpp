#pragma once

//#include <cstring>

#include <memory>
#include <functional>

namespace mlf {

class ArchiveBase {
public:
    ArchiveBase() = default;
    ArchiveBase(ArchiveBase&& other);
    ArchiveBase(const ArchiveBase&) = delete;

public:
    ArchiveBase& operator=(ArchiveBase&& other);
    ArchiveBase& operator=(const ArchiveBase&) = delete;
    ~ArchiveBase();

    char* buffer();
    void set_read_buffer(char* buffer, size_t length, std::function<void(char*)>&& delete_handler);
    void set_write_buffer(char* buffer, size_t capacity, std::function<void(char*)>&& delete_handler);
    void set_buffer(char* buffer, size_t length, size_t capacity, std::function<void(char*)>&& delete_handler);

    char* cursor();
    void set_cursor(char* cursor);
    void advance_cursor(size_t offset);

    char* finish();
    void set_finish(char* finish);
    void advance_finish(size_t offset);

    char* limit();

    size_t position();
    size_t length();
    size_t capacity();
    bool empty();
    void reset();
    void clear();
    char* release();
    void resize(size_t newsize);
    void reserve(size_t newcap);

    void prepare_read(size_t size);
    void prepare_write(size_t size);
    void read(void* data, size_t size);
    void read_back(void* data, size_t size);
    void write(const void* data, size_t size);

protected:
    char* _buffer = NULL;  //buffer head
    char* _cursor = NULL;  //current posotion for read / write
    char* _finish = NULL;  //the end position which filled
    char* _limit = NULL;   //the maximum capacity which >= _finish

    std::function<void(char*)> _delete_handler = nullptr;

    void _free_buffer();
};

} // namespace mlf
