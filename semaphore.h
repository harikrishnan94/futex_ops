#ifndef SEMAPHORE_H
#define SEMAPHORE_H

#include <atomic>

class semaphore
{
public:
    semaphore(int initVal);
    void wait();
    void signal();

private:
    std::atomic_int val;
};

#endif