#include "semaphore.h"

#include <unistd.h>
#include <sys/syscall.h>
#include <linux/futex.h>
#include <sys/time.h>

#include <stdexcept>

#define futex_wait(uaddr, val)          futex(uaddr, FUTEX_WAIT_PRIVATE, val, nullptr, nullptr, 0)
#define futex_wake(uaddr, num_threads)  futex(uaddr, FUTEX_WAKE_PRIVATE, num_threads, nullptr, nullptr, 0)

static int
futex(int *uaddr, int futex_op, int val, const struct timespec *timeout, int *uaddr2, int val3)
{
    return syscall(SYS_futex, uaddr, futex_op, val,
                   timeout, uaddr, val3);
}

semaphore::semaphore(int initVal) : val(initVal)
{
    if (initVal < 0)
        throw std::invalid_argument("Initial Value for semaphore must be greater than -1");
}

void semaphore::wait()
{
    while (1)
    {
        auto oldVal = val.load();

        if (oldVal > 0)
        {
            if (val.compare_exchange_weak(oldVal, oldVal - 1))
                break;
        }
        else
        {
            while (oldVal < 0)
            {
                if (val.compare_exchange_weak(oldVal, oldVal | 0x8000000))
                    break;
            }

            futex_wait(reinterpret_cast<int *>(&val), oldVal);
        }
    }
}

void semaphore::signal()
{
    if (val.fetch_add(1) == 0)
        futex_wake(reinterpret_cast<int *>(&val), 1);
}