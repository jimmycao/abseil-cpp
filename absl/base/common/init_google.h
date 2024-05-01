#pragma once

#include <string>

namespace mlf {

extern bool init_gflags_and_glog(int argc, char* argv[], const std::string & log_name);

} // namespace mlf
