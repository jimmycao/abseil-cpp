#include <glog/logging.h>

#include "log.h"
#include "shell.h"
#include "string_tools.h"

#include "fs_local.h"

namespace mlf {

void LocalFileSystem::mkdir(const std::string& path) {
    if (exists(path, "d")) {
        return;
    } 
    if (exists(path, "f")) {
        remove(path);
    }
    std::string command = format_string("%s mkdir -p %s", _command.c_str(), path.c_str());
    Shell().execute(command, false);
}

std::vector<std::string> LocalFileSystem::list(const std::string& path, int mode) {
    if (exists(path, "e")) {
        std::string command;
        if (mode == 0){
            command = format_string("%s ls %s", _command.c_str(), path.c_str());
        } else {
            command = format_string("%s find %s -type f", _command.c_str(), path.c_str());
        }
        std::string files = trim_spaces(Shell().execute(command, true));
        return split_string(files, '\n');
    }
    return {};
}

std::shared_ptr<FILE> LocalFileSystem::open(const std::string& path,
                                            const std::string& file, 
                                            const std::string& converter, 
                                            const std::string& mode) {
    std::string full_path = path == "" ? file : path + "/" + file;
    std::string direct = mode == "r" ? "<" : ">";
    if (mode == "r") { 
        MLOG(INFO) << "full_path: " << full_path;
        CHECK(exists(full_path, "f"));
    } else {
        mkdir(path);
    }
    std::string command = format_string("%s cat %s %s", 
                                        _command.c_str(), direct.c_str(), full_path.c_str());
    
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

std::string LocalFileSystem::tail(const std::string& path, int n = 1) {
    std::string command = format_string("%s tail -n %d %s", _command.c_str(), n, path.c_str());
    std::string lines = trim_spaces(Shell().execute(command, true));
    return lines;
}

void LocalFileSystem::touch(const std::string& path, const std::string& file) {
    if (exists(path+"/"+file , "d")) {
        remove(path+"/"+file);
    }
    mkdir(path);
    std::string command = format_string("%s touch %s", _command.c_str(), (path+"/"+file).c_str());
    Shell().execute(command, false);
}

bool LocalFileSystem::exists(const std::string& path, const std::string& type) {
    std::string command = format_string("%s [ -%s %s ] ; echo $?", 
                                        _command.c_str(), type.c_str(), path.c_str());
    std::string exists = Shell().execute(command, true);
    return trim_spaces(exists) == "0" ? true : false;
}

void LocalFileSystem::move(const std::string& src, const std::string& dst) {
    std::string command = format_string("%s mv %s %s", _command.c_str(), src.c_str(), dst.c_str());
    Shell().execute(command, false);
}

void LocalFileSystem::remove(const std::string& path) {
    std::string command = format_string("%s rm -rf %s", _command.c_str(), path.c_str());
    Shell().execute(command, false);
}

} // namespace mlf
