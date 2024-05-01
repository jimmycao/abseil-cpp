#include "log.h"
#include "thread_group.h"

namespace mlf {

ThreadGroup::~ThreadGroup() {
    set_real_thread_num(0);
}

void ThreadGroup::set_real_thread_num(int thread_num) {
    MCHECK(thread_num >= 0);
    MCHECK(!joinable());
    if (thread_num == (int)_threads.size()) {
        return;
    }
    _threads = std::vector<ManagedThread>(thread_num);
    _parallel_num = (thread_num == 0) ? 1 : thread_num;
    _barrier.reset(_parallel_num);
};

size_t ThreadGroup::real_thread_num() {
    return _threads.size();
};

void ThreadGroup::set_parallel_num(int parallel_num) {
    MCHECK(parallel_num >= 1);
    set_real_thread_num(parallel_num);
};

int ThreadGroup::parallel_num() {
    return _parallel_num;
};

void ThreadGroup::run(std::function<void (int)> func) {
    start(std::move(func));
    join();
};

void ThreadGroup::start(std::function<void (int)> func) {
    MCHECK((bool)func);
    MCHECK(!joinable());
    if (_threads.empty()) {
        auto old_id = thread_id();
        auto old_grp = parent_group();
        thread_id() = 0;
        parent_group() = this;
        func(0);
        thread_id() = old_id;
        parent_group() = old_grp;
        return;
    }
    _func = std::move(func);
    for (int i = 0; i < _parallel_num; i++) {
        _threads[i].start([this, i]() {
            thread_id() = i;
            parent_group() = this;
            _func(i);
        });
    }
};

bool ThreadGroup::joinable() {
    return (bool)_func;
};

void ThreadGroup::join() {
    MCHECK(joinable());
    for (int i = 0; i < (int)_threads.size(); i++) {
        _threads[i].join();
    }
    _func = nullptr;
};

void ThreadGroup::barrier_wait() {
    _barrier.wait();
};

} // namespace mlf
