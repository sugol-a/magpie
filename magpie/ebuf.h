#ifndef MAGPIE_EBUF_H
#define MAGPIE_EBUF_H

#include <stddef.h>

struct ebuf_entry {
    const char* message;
    const char* function;
    size_t      line;
    void*       blame;
    int         error;
};

#ifndef MAGPIE_EBUF_MAX_ENTRIES
#    define MAGPIE_EBUF_MAX_ENTRIES 16
#endif

#ifndef MAGPIE_EBUF
#    define MAGPIE_EBUF 1
#endif

#ifdef MAGPIE_INTERNAL
#    define EBUF_PUSH(MSG, BLAME)                                             \
        {                                                                     \
            ebuf_push((MSG), __PRETTY_FUNCTION__, __LINE__, (BLAME));         \
        }

void
ebuf_push(const char* message, const char* function, size_t line, void* blame);
#endif

struct ebuf_entry* ebuf_pop(void);

#endif /* MAGPIE_EBUF_H */
