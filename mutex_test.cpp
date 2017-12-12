#include <iostream>
#include "mutex.h"

int main()
{
    mutex m;
    m.lock();
    m.unlock();
}