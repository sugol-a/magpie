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
