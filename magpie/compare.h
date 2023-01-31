#ifndef MAGPIE_COMPARE_H
#define MAGPIE_COMPARE_H

#include <string.h>

static inline int
compare_str(const void* a, const void* b)
{
    const char** str_a = (const char**)a;
    const char** str_b = (const char**)b;

    return strcmp(*str_a, *str_b);
}

#endif /* MAGPIE_COMPARE_H */
