#include "fs.h"

namespace mlf {

size_t& FileSystem::buffer_size() {
    return _buffer_size;
};

std::string& FileSystem::command() {
    return _command;
};

} // namespace mlf
