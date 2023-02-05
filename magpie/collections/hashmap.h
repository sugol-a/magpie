/*
 * Copyright (C) 2023  Alister Sanders
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#ifndef MAGPIE_HASHMAP_H
#define MAGPIE_HASHMAP_H

#include <magpie/collections/array.h>
#include <magpie/collections/list.h>
#include <stdint.h>

#ifndef MAGPIE_HASHMAP_INITIAL_BUCKETS
#    define MAGPIE_HASHMAP_INITIAL_BUCKETS 256
#endif

#ifndef MAGPIE_HASHMAP_LOAD_THRESHOLD
#    define MAGPIE_HASHMAP_LOAD_THRESHOLD 0.75
#endif

struct hashmap {
    struct array buckets;
    size_t       n_entries;
    uint64_t (*hash)(const void*);
    int (*compare)(const void*, const void*);
};

struct hashmap_entry {
    void*    key;
    void*    value;
    uint64_t hash;
    int      alive;
};

struct hashmap_iter {
    struct array*    buckets;
    size_t           bucket;
    struct list_iter current_iter;
};

int hashmap_init(struct hashmap* map,
                 uint64_t (*hash)(const void*),
                 int compare(const void*, const void*));

void hashmap_destroy(struct hashmap* map);

void hashmap_set(struct hashmap* map, void* key, void* value);

struct hashmap_entry* hashmap_lookup(struct hashmap* map, void* key);

int hashmap_get(struct hashmap* map, void* key, void** value);

void hashmap_remove(struct hashmap* map, void* key);

struct hashmap_iter hashmap_iter(struct hashmap* map);

int hashmap_iter_next(struct hashmap_iter* iter);

struct hashmap_entry* hashmap_iter_get(struct hashmap_iter* iter);

#endif /* MAGPIE_HASHMAP_H */
