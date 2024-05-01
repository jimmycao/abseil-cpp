#pragma once

#include <memory>
#include <string>

namespace mlf {

class Shell {
public:
    static Shell& instance() {
        static Shell singleton;
        return singleton;
    }

    Shell& operator()(){
        return Shell::instance();
    }

    std::shared_ptr<FILE> fopen(const std::string& path, const std::string& mode);
    std::shared_ptr<FILE> popen(const std::string& cmd, const std::string& mode);
    std::string execute(const std::string& cmd, bool ret);

private:
    int _popen_fork(const char* cmd, bool read, int parent_end, int child_end);
};

} // namespace mlf
