#include <glog/logging.h>

#include "shell.h"
#include "string_tools.h"

#include "fs_hdfs.h"

namespace mlf {

void HadoopDistributedFileSystem::mkdir(const std::string& path) {
    std::string command = format_string("%s -mkdir %s", _command.c_str(), path.c_str());
    Shell().execute(command, false);
};

std::vector<std::string> HadoopDistributedFileSystem::list(const std::string& path, int mode) {
    if (exists(path, "e")) {
        std::string command;
        if (mode == 0){
            command = format_string("%s -ls %s | grep ^- | awk \'{print $NF}\' ", 
                                    _command.c_str(), path.c_str());
        } else {
            command = format_string("%s -lsr %s | grep ^- | awk \'{print $NF}\' ", 
                                     _command.c_str(), path.c_str());
        }
        std::string files = trim_spaces(Shell().execute(command, true));
        if (files.size() != 0) {
            return split_string(files, '\n');
        } 
    }
    return {};
};

std::shared_ptr<FILE> HadoopDistributedFileSystem::open(const std::string& path,
                                                        const std::string& file, 
                                                        const std::string& converter, 
                                                        const std::string& mode) {
    std::string full_path = path + file; 
    std::string cmd = mode == "r" ? "-cat" : "-put - ";
    if (mode == "r") {
        CHECK(exists(full_path, "e"));
    } else {
        if (!exists(path, "e")) {
            touch(path, file);
        }
        if (exists(full_path, "e")) {
            remove(full_path);
        }
    }
    std::string command = format_string(" %s %s \"%s\"", 
                                        _command.c_str(), cmd.c_str(), full_path.c_str());
    if (converter != "") {
        if (mode == "r") {
            command = format_string("%s | %s", command.c_str(), converter.c_str());
        } else {
            command = format_string("%s | %s", converter.c_str(), command.c_str());
        }
    }
    auto filepointer = Shell().popen(command, mode);
    char* buffer = new char[_buffer_size];
    PCHECK(0 == setvbuf(&*filepointer, buffer, _IOFBF, _buffer_size));
    return {&* filepointer, [filepointer, buffer](FILE*) mutable {
            CHECK(filepointer.unique());
            filepointer = nullptr;
            delete[] buffer;
        }
    };
}

std::string HadoopDistributedFileSystem::tail(const std::string& path, int n = 1) {
    std::string command = format_string("%s -cat %s | tail -n %d", 
                                        _command.c_str(), path.c_str(), n);
    std::string lines = trim_spaces(Shell().execute(command, true));
    return lines;
}

void HadoopDistributedFileSystem::touch(const std::string& path, const std::string& file) {
    std::string command = format_string("%s -touchz %s/%s ", 
                                        _command.c_str(), path.c_str(), file.c_str());
    Shell().execute(command, false);
}

bool HadoopDistributedFileSystem::exists(const std::string& path, const std::string& type) {
    std::string command = format_string("%s -test -%s %s ; echo $?", 
                                         _command.c_str(), type.c_str(), path.c_str());
    std::string exists = Shell().execute(command, true);
    return trim_spaces(exists) == "0" ? true : false;
}

void HadoopDistributedFileSystem::move(const std::string& src, const std::string& dst) {
    std::string command = format_string("%s -mv %s %s", _command.c_str(), src.c_str(), dst.c_str());
    Shell().execute(command, false);
}

void HadoopDistributedFileSystem::remove(const std::string& path) {
    std::string command = format_string("%s -rmr %s", _command.c_str(), path.c_str());
    Shell().execute(command, false);
}

} // namespace mlf
