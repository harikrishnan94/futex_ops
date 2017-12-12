#include "mutex.h"


#include <unistd.h>
#include <sys/syscall.h>
#include <linux/futex.h>
#include <sys/time.h>

#define UNLOCKED			0
#define LOCKED_UNCONTENTED	1
#define LOCKED_CONTENTED	2

#define futex_wait(uaddr, val) futex(uaddr, FUTEX_WAIT_PRIVATE, val, nullptr, nullptr, 0)
#define futex_wake(uaddr, num_threads) futex(uaddr, FUTEX_WAKE_PRIVATE, num_threads, nullptr, nullptr, 0)

static int
futex(int *uaddr, int futex_op, int val, const struct timespec *timeout, int *uaddr2, int val3)
{
    return syscall(SYS_futex, uaddr, futex_op, val,
                   timeout, uaddr, val3);
}

mutex::mutex() : val(UNLOCKED)
{}

void mutex::lock()
{
    int c = UNLOCKED;

    val.compare_exchange_weak(c, LOCKED_UNCONTENTED);

    if (c != UNLOCKED)
    {
        if (c != LOCKED_CONTENTED)
            c = val.exchange(LOCKED_CONTENTED);

        while (c != UNLOCKED)
        {
            futex_wait(reinterpret_cast<int *>(&val), LOCKED_CONTENTED);
            c = val.exchange(LOCKED_CONTENTED);
        }
    }
}

void mutex::unlock()
{
    if (val.fetch_sub(1) != LOCKED_UNCONTENTED)
    {
        val = UNLOCKED;
        futex_wake(reinterpret_cast<int *>(&val), 1);
    }
}