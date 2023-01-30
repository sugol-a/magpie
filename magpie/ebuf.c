#include <errno.h>
#include <string.h>

#define MAGPIE_INTERNAL 1
#include <magpie/ebuf.h>

struct ebuf_entry ebuf[MAGPIE_EBUF_MAX_ENTRIES];
size_t            ebuf_entries = 0;

#ifdef MAGPIE_EBUF
void
ebuf_push(const char* message, const char* function, size_t line, void* blame)
{
    struct ebuf_entry entry = {
        .message  = message,
        .function = function,
        .line     = line,
        .blame    = blame,
        .error    = errno,
    };

    if (ebuf_entries == MAGPIE_EBUF_MAX_ENTRIES) {
        memmove(ebuf, ebuf + 1, sizeof(ebuf) - sizeof(struct ebuf_entry));
        ebuf_entries--;
    }

    ebuf[ebuf_entries++] = entry;
}

struct ebuf_entry*
ebuf_pop(void)
{
    if (ebuf_entries == 0) {
        return NULL;
    }

    return &ebuf[--ebuf_entries];
}

#else

void
ebuf_push(const char* _message,
          const char* _function,
          size_t      _line,
          void*       _blame)
{
    /* nop */
}

struct ebuf_entry*
ebuf_pop(void)
{
    return NULL;
}

#endif
