#pragma once

#include <stdlib.h>
#include <string>
#include <sstream>

namespace mlf {

// level: INFO, WARNING, ERROR, FATAL : 0, 1, 2, 3
extern void init_glog_conf(bool log_to_stderr = true, int min_stderr_level = 0, 
                           int min_log_level = 0, const std::string & log_dir = "./log/");

class Logger {
public:
    Logger(const char* file, int line, const char* func, const char* level);
    ~Logger();

    template<class BASIC_TYPE>
    Logger& operator << (const BASIC_TYPE & v) {
        _buffer << v;
        return *this;
    }

private:
    const char* _file;
    int _line;
    const char* _func;
    std::string _level;

    std::stringstream _buffer;    
};

/* refer from glog/logging.h */
class MLogMessageVoidify {
public:
    MLogMessageVoidify() { }
    // This has to be an operator with a precedence lower than << but
    // higher than ?:
    void operator&(Logger&) { }
};

#ifndef M_PREDICT_BRANCH_NOT_TAKEN
    #if 1
        #define M_PREDICT_BRANCH_NOT_TAKEN(x) (__builtin_expect(x, 0))
    #else
        #define M_PREDICT_BRANCH_NOT_TAKEN(x) x
    #endif
#endif

#define MLOG(level) \
    ::mlf::Logger(__FILE__, __LINE__, __func__, #level)

#define MLOG_IF(severity, condition) \
    static_cast<void>(0),             \
    !(condition) ? (void) 0 : ::mlf::MLogMessageVoidify() & MLOG(severity)

#define MCHECK(condition)  \
    MLOG_IF(FATAL, M_PREDICT_BRANCH_NOT_TAKEN(!(condition))) \
             << "Check failed: " #condition " "

#ifndef CHECK
    #define CHECK(condition) MCHECK(condition)
#endif

}
