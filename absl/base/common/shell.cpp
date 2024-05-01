#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/syscall.h>

#include <glog/logging.h>

#include "line_reader.h"

#include "shell.h"

namespace mlf {

static void close_open_fds_internal() {
    struct linux_dirent {
        long d_ino = 0;
        off_t d_off;
        unsigned short d_reclen = 0;
        char d_name[256];
    };

    int dir_fd = -1;
    PCHECK((dir_fd = open("/proc/self/fd", O_RDONLY)) >= 0);
    char buffer[sizeof(linux_dirent)];

    for (;;) {
        int bytes = syscall(SYS_getdents, dir_fd, (linux_dirent*)buffer, sizeof(buffer));
        CHECK(bytes >= 0);
        if (bytes == 0) {
            break;
        }
        linux_dirent* entry = NULL;
        for (int offset = 0; offset < bytes; offset += entry->d_reclen) {
            entry = (linux_dirent*)(buffer + offset);
            int fd = 0;
            const char* s = entry->d_name;
            while (*s >= '0' && *s <= '9') {
                fd = fd * 10 + (*s - '0');
                s++;
            }
            if (s != entry->d_name && fd != dir_fd && fd >= 3) {
                close(fd);
            }
        }
    }
    close(dir_fd);
}

std::shared_ptr<FILE> Shell::fopen(const std::string& path, const std::string& mode) {
    FILE* fp = ::fopen(path.c_str(), mode.c_str());
    CHECK(fp);
    return {fp, [this, path](FILE* fp) {
        PCHECK((0 == fclose(fp)));
    }};
};

int Shell::_popen_fork(const char* cmd, bool read, int parent_end, int child_end) {
    int child_pid = -1;
    PCHECK((child_pid = vfork()) >= 0);

    if (child_pid != 0) {
        return child_pid;
    }

    int child_std_end = read ? 1 : 0;
    close(parent_end);

    if (child_end != child_std_end) {
        PCHECK(dup2(child_end, child_std_end) == child_std_end);
        close(child_end);
    }

    close_open_fds_internal();
    PCHECK(execl("/bin/sh", "sh", "-c", cmd, NULL) >= 0);
    exit(127);
};

std::shared_ptr<FILE> Shell::popen(const std::string& cmd, const std::string& mode) {
    CHECK(mode == "r" || mode == "w");
    bool read = mode == "r";

    int pipe_fds[2];
    PCHECK(pipe(pipe_fds) == 0);
    
    int parent_end = 0;
    int child_end = 0;

    if (read) {
        parent_end = pipe_fds[0];
        child_end = pipe_fds[1];
    } else {
        parent_end = pipe_fds[1];
        child_end = pipe_fds[0];
    }

    int child_pid = _popen_fork(cmd.c_str(), read, parent_end, child_end);

    close(child_end);
    fcntl(parent_end, F_SETFD, FD_CLOEXEC);
    FILE* fp = fdopen(parent_end, mode.c_str());
    CHECK(fp != NULL);

    return { 
        fp, [child_pid, cmd] (FILE * fp) {
            PCHECK(fclose(fp) == 0);
            int wstatus = -1;
            int ret = -1;

            do {
                PCHECK((ret = waitpid(child_pid, &wstatus, 0)) >= 0 || (ret == -1 && errno == EINTR));
            } while (ret == -1 && errno == EINTR);
            CHECK( (wstatus == 0) || 
                    (wstatus == 256) ||
                    wstatus == (128 + SIGPIPE) * 256 || 
                    (wstatus == -1 && errno == ECHILD) ||
                    (wstatus == -1 && errno == ECHILD));
        }
    };
};

std::string Shell::execute(const std::string& cmd, bool ret = false) {
    auto pipe = ret ? Shell().popen(cmd, "r") : Shell().popen(cmd, "w");
    if (!ret) {
        return "";
    } else {
        LineReader reader;
        if (reader.getdelim(&*pipe, 0)) {
            return reader.get();
        } else {
            return "";
        }
    }
}

} // namespace mlf
