#include <magpie/math/prime.h>

int
is_prime(size_t x)
{
    int prime = 1;
    
    if ((x % 2 == 0 && x != 2) || (x < 2)) {
        return 0;
    }
    
    for (size_t i = 3; i < x; i += 2) {
        if (x % i == 0) {
            prime = 0;
            break;
        }
    }

    return prime;
}

size_t
next_prime(size_t after)
{
    size_t next = after + 1;
    if (next % 2 == 0) {
        next += 1;
    }

    while (!is_prime(next)) {
        next += 2;
    }

    return next;
}
