#include <glog/logging.h>

#include "thread_barrier.h"

namespace mlf {

ThreadBarrier::~ThreadBarrier() {
    PCHECK(0 == pthread_barrier_destroy(&_barrier));
}

void ThreadBarrier::reset(int count) {
    CHECK(count >= 1);
    PCHECK(0 == pthread_barrier_destroy(&_barrier));
    PCHECK(0 == pthread_barrier_init(&_barrier, NULL, count));
};

void ThreadBarrier::wait() {
    int err = pthread_barrier_wait(&_barrier);
    PCHECK((err = pthread_barrier_wait(&_barrier), err == 0 || err == PTHREAD_BARRIER_SERIAL_THREAD));
};

} // namespace mlf
