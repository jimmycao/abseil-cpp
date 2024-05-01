#pragma once

#include <atomic>
#include <vector>

#include "common/thread_barrier.h"
#include "common/thread_managed.h"

namespace mlf {

/**
 * ThreadGroup holds a number of ManagedThread, 
 * each ManagedThread has a thread_id, and run a std::function<void (int)> _func.
 *
 * when 'start(func)', a number of thread will start to run _func;
 * when join(), a number of thread will join.
 */
class ThreadGroup {
public:
    explicit ThreadGroup(int thread_num = 0) {
        set_real_thread_num(thread_num);
    }
    ThreadGroup(const ThreadGroup&) = delete;
    ThreadGroup& operator=(const ThreadGroup&) = delete;
    ~ThreadGroup();
    
    void set_real_thread_num(int thread_num);
    size_t real_thread_num();

    void set_parallel_num(int parallel_num);
    int parallel_num();

    void run(std::function<void (int)> func);  //start, and then join
    void start(std::function<void (int)> func);
    bool joinable();
    void join();

    void barrier_wait();

    static int& thread_id() {
        thread_local int x = 0;
        return x;
    }

    static ThreadGroup*& parent_group() {
        thread_local ThreadGroup* x = NULL;
        return x;
    }

private:
    std::vector<ManagedThread> _threads;
    int _parallel_num = 1;

    ThreadBarrier _barrier;
    std::function<void (int)> _func;
};

} // namespace mlf
