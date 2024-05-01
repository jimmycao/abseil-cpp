#include "log.h"
#include "thread_managed.h"

namespace mlf {

ManagedThread::ManagedThread() {
    _thread = std::thread([this]() {
        _run_thread();
    });
}

ManagedThread::~ManagedThread() {
    MCHECK(!_active);
    _terminate = true;
    _sem_start.post();
    _thread.join();
}

void ManagedThread::start(std::function<void()> && task_fn) {
    MCHECK(!_active);
    _active = true;
    _task_fn = std::move(task_fn);
    _sem_start.post();
}

void ManagedThread::join() {
    MCHECK(_active);
    _sem_finish.wait(); //since _sem_start is init with 0, just wait until _run_thread post it
    _active = false;
}

bool ManagedThread::active() {
    return _active;
}

void ManagedThread::_run_thread() {
    while (!_terminate) {
        _sem_start.wait();  //since _sem_start is 0, just wait until start() to post it
        if (_terminate) {
            break;
        };
        _task_fn();
        _sem_finish.post();
    }
}

} // namespace mlf
