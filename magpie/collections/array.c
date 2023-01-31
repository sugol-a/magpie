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

#include <string.h>
#define _GNU_SOURCE 1

#include <assert.h>
#include <stdlib.h>

#define MAGPIE_INTERNAL 1

#include <magpie/collections/array.h>
#include <magpie/ebuf.h>

struct sort_inner {
    int (*compare)(const void* a, const void* b);
    int direction;
};

static int resize(struct array* a, size_t min_capacity);

static void insertion_sort(struct array* a,
                           int (*compare)(const void*, const void*),
                           int direction);

static void quicksort(struct array* a,
                      int (*compare)(const void*, const void*),
                      int direction);

static int compare_inner(const void* a, const void* b, void* extra);

static int compare_ptr(const void* a, const void* b);

static inline void swap(void** a, void** b);

static ssize_t binary_search(struct array* a,
                             size_t        min,
                             size_t        max,
                             int (*compare)(const void*, const void*),
                             const void* element);

int
array_init(struct array* a)
{
    return array_init_with_capacity(a, MAGPIE_DEFAULT_ARRAY_CAPACITY);
}

int
array_init_with_capacity(struct array* a, size_t capacity)
{
    a->elements = malloc(sizeof(*a->elements) * capacity);

    if (a->elements == NULL) {
        a->capacity = 0;
        return 0;
    }

    a->capacity = capacity;
    a->length   = 0;

    return 1;
}

void
array_destroy(struct array* a)
{
    free(a->elements);

    a->elements = NULL;
    a->capacity = 0;
    a->length   = 0;
}

void
array_push(struct array* a, void* element)
{
    if (a->length >= a->capacity) {
        if (!resize(a, a->length + 1)) {
            EBUF_PUSH("failed to push element", a);
            return;
        }
    }

    a->elements[a->length++] = element;
}

int
array_pop(struct array* a, void** element)
{
    if (a->length == 0) {
        if (element != NULL) {
            *element = NULL;
        }

        return 0;
    }

    a->length--;

    if (element != NULL) {
        *element = a->elements[a->length];
    }

    return 1;
}

void
array_insert(struct array* a, size_t index, void* element)
{
    if (a->length >= a->capacity) {
        if (!resize(a, a->length + 1)) {
            EBUF_PUSH("failed to insert element", a);
            return;
        }
    }

    memmove(&a->elements[index + 1], &a->elements[index], sizeof(*a->elements) * (a->length - index));
    a->elements[index] = element;
    a->length++;
}

void
array_insert_sorted(struct array* a,
                    int (*compare)(const void*, const void*),
                    void* element)
{
    ssize_t index = binary_search(a, 0, a->length - 1, compare, element);
    array_insert(a, index, element);
}

int
array_remove(struct array* a, size_t index, void** element)
{
    if (index >= a->length) {
        if (element != NULL) {
            *element = NULL;
        }

        return 0;
    }

    if (element != NULL) {
        *element = a->elements[index];
    }

    memmove(&a->elements[index], &a->elements[index + 1], sizeof(*a->elements) * (a->length - index));
    a->length--;

    return 1;
}

void
array_clear(struct array* a)
{
    a->length = 0;
}

void
array_sort(struct array* a,
           int (*compare)(const void*, const void*),
           int sort,
           int direction)
{
    switch (sort) {
        case ARRAY_INSERTION_SORT:
            insertion_sort(a, compare, direction);
            break;

        case ARRAY_QUICKSORT: quicksort(a, compare, direction); break;

        default: EBUF_PUSH("invalid sorting algorithm specified", a); break;
    }
}

ssize_t
array_find(struct array* a, const void* element)
{
    return array_find_by(a, compare_ptr, element);
}

ssize_t
array_find_by(struct array* a,
              int (*compare)(const void*, const void*),
              const void* element)
{
    ssize_t index = -1;

    for (size_t i = 0; i < a->length; i++) {
        if (compare(a->elements[i], element) == 0) {
            index = i;
            break;
        }
    }

    return index;
}

ssize_t
array_binary_search(struct array* a,
                    int (*compare)(const void*, const void*),
                    const void* element)
{
    ssize_t index = binary_search(a, 0, a->length - 1, compare, element);
    return compare(&a->elements[index], &element) == 0 ? index : -1;
}

static int
resize(struct array* a, size_t min_capacity)
{
    void*  temp;
    size_t capacity = a->capacity;

    while (capacity < min_capacity) {
        capacity *= 2;
    }

    temp = realloc(a->elements, sizeof(*a->elements) * capacity);

    if (temp == NULL) {
        EBUF_PUSH("failed to reallocate array", a);
        return 0;
    }

    a->elements = temp;
    a->capacity = capacity;

    return 1;
}

static void
insertion_sort(struct array* a,
               int (*compare)(const void*, const void*),
               int direction)
{
    struct sort_inner inner = { .compare = compare, .direction = direction };
    size_t            i     = 1;

    while (i < a->length) {
        size_t j = i;

        while (j > 0
               && compare_inner(&a->elements[j - 1], &a->elements[j], &inner)
                      > 0) {
            swap(&a->elements[j - 1], &a->elements[j]);
            j--;
        }

        i++;
    }
}

static void
quicksort(struct array* a,
          int (*compare)(const void*, const void*),
          int direction)
{
    struct sort_inner inner = { .compare = compare, .direction = direction };

    qsort_r(a->elements,
            a->length,
            sizeof(*a->elements),
            compare_inner,
            &inner);
}

static int
compare_inner(const void* a, const void* b, void* extra)
{
    const struct sort_inner* inner = extra;

    switch (inner->direction) {
        case ARRAY_SORT_ASCENDING: return inner->compare(a, b);

        case ARRAY_SORT_DESCENDING: return 1 - inner->compare(a, b);
    }

    return 0;
}

static int
compare_ptr(const void* a, const void* b)
{
    return a == b ? 0 : 1;
}

static inline void
swap(void** a, void** b)
{
    void* temp = *a;
    *a         = *b;
    *b         = temp;
}

static ssize_t
binary_search(struct array* a,
              size_t        min,
              size_t        max,
              int (*compare)(const void*, const void*),
              const void* element)
{
    int    cmp;
    size_t midpoint = (max + min) / 2;

    cmp = compare(&a->elements[midpoint], &element);

    if (min - max <= 1) {
        return max;
    }

    if (cmp > 0) {
        return binary_search(a, min, midpoint - 1, compare, element);
    }
    else if (cmp < 0) {
        return binary_search(a, midpoint + 1, max, compare, element);
    }
    else {
        return midpoint;
    }
}
