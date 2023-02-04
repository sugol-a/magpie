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

#include <stddef.h>
#include <stdlib.h>

#define MAGPIE_INTERNAL 1
#include <magpie/collections/hashmap.h>
#include <magpie/collections/list.h>
#include <magpie/ebuf.h>
#include <magpie/math/prime.h>

static size_t
initial_buckets(void)
{
    static size_t n = 0;

    if (n == 0) {
        n = next_prime(MAGPIE_HASHMAP_INITIAL_BUCKETS);
    }

    return n;
}

static inline size_t
bucket_for_hash(struct hashmap* map, uint64_t key_hash)
{
    return key_hash % map->buckets.length;
}

static struct hashmap_entry*
lookup(struct hashmap* map, void* key, uint64_t key_hash)
{
    size_t bucket = bucket_for_hash(map, key_hash);

    struct list*     head = map->buckets.elements[bucket];
    struct list_iter it   = list_iter(head);

    /* the head always contains NULL, skip it */
    list_iter_next(&it);

    while (list_iter_next(&it)) {
        struct list*          item  = list_iter_get(&it);
        struct hashmap_entry* entry = item->data;

        if (entry->hash == key_hash && map->compare(&entry->key, &key) == 0) {
            return entry;
        }
    }

    return NULL;
}

static int
insert(struct hashmap* map, void* key, uint64_t key_hash, void* value)
{
    size_t bucket = bucket_for_hash(map, key_hash);

    struct hashmap_entry* entry = NULL;
    struct list*          head  = map->buckets.elements[bucket];
    struct list_iter      it    = list_iter(head);

    /* skip the head */
    list_iter_next(&it);

    /* Look for a tombstone to place the item in first */
    while (list_iter_next(&it)) {
        struct list*          l = list_iter_get(&it);
        struct hashmap_entry* e = l->data;

        if (!e->alive) {
            entry = e;
            break;
        }
    }

    if (entry == NULL) {
        struct list* entry_item;

        entry      = malloc(sizeof(*entry));
        entry_item = list_alloc(entry);

        list_append(head, entry_item);
    }

    entry->key   = key;
    entry->value = value;
    entry->hash  = key_hash;
    entry->alive = 1;

    /* TODO: return 1 if we need to rehash */
    return 0;
}

int
hashmap_init(struct hashmap* map,
             uint64_t (*hash)(const void*),
             int (*compare)(const void*, const void*))
{
    const size_t buckets = initial_buckets();
    array_init_with_capacity(&map->buckets, buckets);

    for (size_t i = 0; i < buckets; i++) {
        array_push(&map->buckets, list_alloc(NULL));
    }

    map->hash    = hash;
    map->compare = compare;

    return 1;
}

void
hashmap_destroy(struct hashmap* map)
{
    for (size_t i = 0; i < map->buckets.length; i++) {
        struct list*     head = map->buckets.elements[i];
        struct list_iter it   = list_iter(head);

        while (list_iter_next(&it)) {
            struct list* item = list_iter_get(&it);
            free(item->data);
            free(item);
        }

        map->buckets.elements[i] = NULL;
    }

    array_destroy(&map->buckets);
}

void
hashmap_set(struct hashmap* map, void* key, void* value)
{
    uint64_t key_hash = map->hash(&key);

    /* Check whether the hashmap already contains an entry for this key */
    struct hashmap_entry* entry = lookup(map, key, key_hash);

    if (entry == NULL) {
        insert(map, key, key_hash, value);
    }
    else {
        entry->value = value;
        entry->alive = 1;
    }
}

struct hashmap_entry*
hashmap_lookup(struct hashmap* map, void* key)
{
    uint64_t              key_hash = map->hash(&key);
    struct hashmap_entry* entry    = lookup(map, key, key_hash);

    if (entry == NULL || !entry->alive) {
        return NULL;
    }

    return entry;
}

int
hashmap_get(struct hashmap* map, void* key, void** value)
{
    struct hashmap_entry* entry = hashmap_lookup(map, key);

    if (entry == NULL) {
        *value = NULL;
        return 0;
    }

    *value = entry->value;
    return 1;
}

void
hashmap_remove(struct hashmap* map, void* key)
{
    struct hashmap_entry* entry = hashmap_lookup(map, key);

    if (entry == NULL) {
        return;
    }

    entry->alive = 0;
}

struct hashmap_iter
hashmap_iter(struct hashmap* map)
{
    struct hashmap_iter iter = {
        .buckets      = &map->buckets,
        .bucket       = 0,
        .current_iter = {.i = -1, .current = NULL, .pending = NULL},
    };

    return iter;
}

int
hashmap_iter_next(struct hashmap_iter* iter)
{
    int has_next = 0;

    while (!has_next) {
        struct list*          list_entry;
        struct hashmap_entry* entry;

        if (iter->current_iter.i < 0) {
            if (iter->bucket >= iter->buckets->length) {
                break;
            }

            iter->current_iter
                = list_iter(iter->buckets->elements[iter->bucket++]);

            /* skip list head */
            list_iter_next(&iter->current_iter);
        }

        if (!list_iter_next(&iter->current_iter)) {
            iter->current_iter.i = -1;
            continue;
        }

        list_entry = list_iter_get(&iter->current_iter);
        entry      = list_entry->data;

        if (!entry->alive) {
            continue;
        }

        has_next = 1;
    }

    return has_next;
}

struct hashmap_entry*
hashmap_iter_get(struct hashmap_iter* iter)
{
    struct list* list_entry = list_iter_get(&iter->current_iter);

    if (list_entry == NULL) {
        return NULL;
    }

    return list_entry->data;
}
