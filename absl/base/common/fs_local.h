#pragma once

#include <memory>
#include <string>
#include <vector>

#include "common/fs.h"

namespace mlf {

class LocalFileSystem : public FileSystem {
public:
    void mkdir(const std::string& path);
    std::vector<std::string> list(const std::string& path, int mode);

    std::shared_ptr<::FILE> open(const std::string& path, 
                                         const std::string& file, 
                                         const std::string& converter, 
                                         const std::string& mode);
    std::string tail(const std::string& file, int n);
    void touch(const std::string& path, const std::string& file);

    bool exists(const std::string& path, const std::string& type);

    void move(const std::string& src, const std::string& dst);
    void remove(const std::string& path);
};

using LFS = LocalFileSystem;

} // namespace mlf
