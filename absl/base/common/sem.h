#pragma once

#include <semaphore.h>

namespace mlf {

class Semaphore {
public:
   /**
    * #include <semaphore.h>
    * int sem_init(sem_t *sem, int pshared, unsigned int value);
    *
    * pshared:
    * Nonzero if you want the semaphore to be shared between processes via shared memory.
    * If the value of pshared is zero, then the semaphore cannot be shared between processes.
    * If the value of pshared is nonzero, then the semaphore can be shared between processes.
    *
    * value:
    * deadlock waiting until value is changed to bigger than 0 by another thread
    * The initial value of the semaphore. A positive value (i.e. greater than zero)
    * indicates an unlocked semaphore, and a value of 0 (zero) indicates a locked semaphore.
    * This value must not exceed SEM_VALUE_MAX.
    */
    Semaphore(int pshared=0, int value=1);
    Semaphore(const Semaphore&) = delete;
    Semaphore& operator=(const Semaphore&) = delete;
    ~Semaphore();

    void post();
    void wait();
    bool try_wait();

private:
    sem_t _sem;
};

} // namespace mlf
