#pragma once

#include <thread>
#include <functional>

#include "common/sem.h"

namespace mlf {

// Only moveable
class alignas(64) ManagedThread {
public:
    ManagedThread();
    ManagedThread(const ManagedThread&) = delete;
    ManagedThread& operator=(const ManagedThread&) = delete;
    ~ManagedThread();

    void start(std::function<void()> && task_fn);
    void join();

    bool active();

private:
    void _run_thread();

private:
    bool _active = false;
    bool _terminate = false;
    Semaphore _sem_start {0, 0};
    Semaphore _sem_finish {0, 0};

    std::thread _thread;
    std::function<void()> _task_fn;
};

} // namespace mlf
