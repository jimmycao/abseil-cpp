#include "log.h"
#include "sem.h"

namespace mlf {

Semaphore::Semaphore(int pshared, int value) { 
    MCHECK(0 == sem_init(&_sem, pshared, value));
}

Semaphore::~Semaphore() {
    MCHECK(0 == sem_destroy(&_sem));
}

void Semaphore::post() {
    MCHECK(0 == sem_post(&_sem));
};

void Semaphore::wait() {
    sem_wait(&_sem);
};

bool Semaphore::try_wait() {
    return sem_trywait(&_sem);
};

} // namespace mlf
