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

#ifndef MAGPIE_COLLECTION_INTEROP_H
#define MAGPIE_COLLECTION_INTEROP_H

#include <magpie/collections/array.h>
#include <magpie/collections/list.h>

/**
 * Converts an array to a linked list.
 *
 * Allocates a new linked list and appends all items in `src` to
 * it. List items are dynamically allocated, so the list must be
 * `free()`'d.
 *
 * @param `dest` :: Pointer to a linked list. `*dest` may be `NULL`.
 * @param `src` :: Pointer to the array to convert.
 * @return 0 on error, non-zero on success.
 */
int array_to_list(struct list** dest, struct array* src);

/**
 * Converts a linked list to an array.
 *
 * Allocates a new array and appends all items in `src` to it. The
 * array must be destroyed (see `array_destroy()`) after use.
 *
 * @param `dest` :: Pointer to an array.
 * @param `src` :: Pointer to the linked list to convert.
 * @return 0 on error (currently never happens), or non-zero on success.
 */
int list_to_array(struct array* dest, struct list* src);

#endif /* MAGPIE_COLLECTION_INTEROP_H */
