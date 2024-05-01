#pragma once

#include <string>
#include <vector>
#include <memory>

namespace mlf {

class FileSystem {
public:
    virtual void mkdir(const std::string& path) = 0;
    virtual std::vector<std::string> list(const std::string& path, int mode) = 0;

    virtual std::shared_ptr<::FILE> open(const std::string& path, 
                                         const std::string& file, 
                                         const std::string& converter, 
                                         const std::string& mode) = 0;
    virtual std::string tail(const std::string& file, int n = 1) = 0;
    virtual void touch(const std::string& path, const std::string& file) = 0;
    
    virtual bool exists(const std::string& path, const std::string& type) = 0;

    virtual void move(const std::string& src, const std::string& dst) = 0;
    virtual void remove(const std::string& path) = 0;

    size_t& buffer_size();
    std::string& command();
    
protected:
    size_t _buffer_size = 2^24;
    std::string _command = "";
};

} // namespace mlf
