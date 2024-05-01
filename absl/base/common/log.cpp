#include <glog/logging.h>
#include <gflags/gflags.h>

#include "log.h"
#include "string_tools.h"

namespace mlf {
 
// level: INFO, WARNING, ERROR, FATAL : 0, 1, 2, 3
void init_glog_conf(bool log_to_stderr, int min_stderr_level, 
                   int min_log_level, const std::string & log_dir) {
    FLAGS_alsologtostderr = log_to_stderr; 
    FLAGS_stderrthreshold = min_stderr_level; // print to stderr
    FLAGS_minloglevel = min_log_level;
    FLAGS_logbuflevel = -1; //default 0, will cache INFO log
    FLAGS_log_dir = log_dir;
}

Logger::Logger(const char * file, int line, const char * func, const char * level) :  
        _level(level), _file(file), _line(line), _func(func) {
}

Logger::~Logger() {
    if (_level == "WARNING") {
        auto msg = google::LogMessage(_file, _line, google::GLOG_WARNING);
        msg.stream() << _func << "():" << _buffer.str();
    } else if (_level == "FATAL") {
        auto msg = google::LogMessage(_file, _line, google::GLOG_FATAL);
        msg.stream() << _func << "():" << _buffer.str();
    } else if (_level == "ERROR") {
        auto msg = google::LogMessage(_file, _line, google::GLOG_ERROR);
        msg.stream() << _func << "():" << _buffer.str();
    } else {
        auto msg = google::LogMessage(_file, _line, google::GLOG_INFO);
        msg.stream() << _func << "():" << _buffer.str();
    }
}

} //namespace mlf
