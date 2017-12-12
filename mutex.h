#ifndef MUTEX_H
#define MUTEX_H

#include <atomic>

class mutex
{
public:
    mutex();
    void lock();
    void unlock();

private:
    std::atomic_int val;
};

#endif