#pragma once

#include <pthread.h>
#include "common/log.h"

namespace mlf {

class ThreadBarrier {
public:
    explicit ThreadBarrier(int count = 1) {
        MCHECK(count >= 1);
        MCHECK(0 == pthread_barrier_init(&_barrier, NULL, count));
    }
    ThreadBarrier(const ThreadBarrier&) = delete;
    ThreadBarrier& operator=(const ThreadBarrier&) = delete;

    ~ThreadBarrier();
    
    void reset(int count);
    void wait();

private:
    pthread_barrier_t _barrier;
};

} // namespace mlf
