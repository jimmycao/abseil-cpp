#pragma once

#include <map>
#include <limits>
#include <thread>
#include <atomic>
#include <mutex>
#include <string>
#include <sstream>
#include <vector>
#include <functional>
#include <unordered_map>

#include "log.h"

namespace mlf {

typedef std::pair<std::string, std::string> tag_pair_t;
typedef std::pair<std::string, int> port_pair_t;

struct HostIns {
    std::string service_name;
    std::string type;
    std::string host_name;
    std::string host_ip;
    std::string deploy_path;
    std::string run_user;
    std::vector<port_pair_t> ports;

    std::map<std::string, std::string> tags;
    std::string health_check_cmd;
    std::string health_check_type;
    int rank;
    int instance_id;
    int baidu_node_id;
    int status;
    int client_id;

    std::string to_str() {
        std::ostringstream os;
        os << "host_ip[" << host_ip << "]. ports[";
        for (size_t i = 0; i < ports.size(); ++i) {
            os << ports[i].first << ':' << ports[i].second << ';';
        }

        os << "]. client_id[" << client_id << "]. ";
        return os.str();
    }
};

template<class AR>
AR& operator<<(AR& ar, const HostIns& val) {
    ar << val.service_name << val.type << val.host_name 
       << val.host_ip << val.deploy_path << val.run_user;
    ar << val.health_check_cmd << val.health_check_type << val.rank 
       << val.instance_id << val.baidu_node_id << val.status;
    ar << (int)(val.ports.size());
    for (size_t i = 0; i < val.ports.size(); ++i) {
        ar << val.ports[i].first << val.ports[i].second;
    }

    ar << val.client_id;
    return ar;
}

template<class AR>
AR& operator>>(AR& ar, HostIns& val) {
    ar >> val.service_name >> val.type >> val.host_name 
       >> val.host_ip >> val.deploy_path >> val.run_user;
    ar >> val.health_check_cmd >> val.health_check_type >> val.rank 
       >> val.instance_id >> val.baidu_node_id >> val.status;
    int port_size = 0;
    ar >> port_size;
    for (size_t i = 0; i < port_size; ++i) {
        std::string key;
        int value;
        ar >> key >> value;
        val.ports.push_back(std::make_pair(key, value));
    }
    
    ar >> val.client_id;
    return ar;
}

// Call func(args...). If interrupted by signal, recall the function.
template<class FUNC, class... ARGS>
auto ignore_signal_call(FUNC func, ARGS && ... args) -> typename std::result_of<FUNC(ARGS...)>::type {
    for (;;) {
        auto err = func(args...);

        if (err < 0 && (errno == EINTR || errno == EAGAIN)) {
            MLOG(NOTICE) << errno << " signal is caught. Ignored.";
            continue;
        }

        return err;
    }
}

inline std::mutex& global_fork_mutex() {
    static std::mutex mutex;
    return mutex;
}

// P or L标识当前程序是pserver or learner
inline char& global_pserver_or_learner() {
    static char pserver_or_learner = 'P';
    return pserver_or_learner;
}

inline bool is_table() {
    return global_pserver_or_learner() == 'P';
}

// popen and pclose are not thread-safe
inline FILE* guarded_popen(const char* command, const char* type) {
    std::lock_guard<std::mutex> lock(global_fork_mutex());
    return popen(command, type);
}

inline int guarded_pclose(FILE* stream) {
    std::lock_guard<std::mutex> lock(global_fork_mutex());
    return pclose(stream);
}

} //namespace mlf
