#include <glog/logging.h>
#include <gflags/gflags.h>

#include "common/log.h"
#include "common/fs_local.h"
#include "common/string_tools.h"

#include "common/init_google.h"

namespace mlf {

bool init_gflags_and_glog(int argc, char* argv[], const std::string & log_name) {
    mlf::LocalFileSystem local_fs;
    int pid = getpid();
    std::string log_dir = format_string("./log/%s_%d", log_name.c_str(), pid);
    local_fs.remove(log_dir);
    local_fs.mkdir(log_dir);

    bool log_to_stderr = true;
    int min_stderr_level = 0;
    int min_log_level = 0;
    mlf::init_glog_conf(log_to_stderr, min_stderr_level, min_log_level, log_dir);

    google::ParseCommandLineFlags(&argc, &argv, true);
    google::InitGoogleLogging(argv[0]);

    google::CommandLineFlagInfo info;
    if (google::GetCommandLineFlagInfo("crash_on_fatal_log", &info) && info.is_default) {
        if (google::SetCommandLineOption("crash_on_fatal_log", "true").empty()) {
            MLOG(FATAL) << "Fail to set -crash_on_fatal_log";
            return false;
        }
        google::GetCommandLineFlagInfo("crash_on_fatal_log", &info); 
    }
    MLOG(INFO) << "gflags:crash_on_fatal_log=" << info.current_value;

    if (google::GetCommandLineFlagInfo("usercode_in_pthread", &info) && info.is_default) {
        if (google::SetCommandLineOption("usercode_in_pthread", "true").empty()) {
            MLOG(FATAL) << "Fail to set -usercode_in_pthread";
            return false;
        }
        google::GetCommandLineFlagInfo("usercode_in_pthread", &info);
    }
    MLOG(INFO) << "gflags:usercode_in_pthread=" << info.current_value;

    if (google::GetCommandLineFlagInfo("bthread_concurrency", &info) && info.is_default) {
        if (google::SetCommandLineOption("bthread_concurrency", "512").empty()) {
            MLOG(FATAL) << "Fail to set -bthread_concurrency";
            return false;
        } 
        google::GetCommandLineFlagInfo("bthread_concurrency", &info);
    }
    MLOG(INFO) << "gflags:bthread_concurrency=" << info.current_value;

    if (google::GetCommandLineFlagInfo("max_body_size", &info) && info.is_default) {
        if (google::SetCommandLineOption("max_body_size", "10240000000").empty()) {
            MLOG(FATAL) << "Fail to set -max_body_size";
            return false;
        }
        google::GetCommandLineFlagInfo("max_body_size", &info); 
    }
    MLOG(INFO) << "gflags:max_body_size=" << info.current_value;

    return true;
}


} // namespace mlf
