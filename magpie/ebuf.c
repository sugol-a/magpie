/*
 * Copyright (C) 2023  Alister Sanders
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

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
