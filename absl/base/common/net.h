#pragma once

#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>

#include <string>

#include "common/log.h"

namespace mlf {

std::string get_ip();

std::string get_hostname();

int get_local_port();

} // namespace mlf
