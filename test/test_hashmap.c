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

#include <stdlib.h>

#include "test_common.h"
#include <CUnit/Basic.h>
#include <magpie/collections/hashmap.h>
#include <magpie/collections/list.h>
#include <magpie/compare.h>
#include <magpie/hash.h>

#include "hashmap_entries.h"

struct entry_visited {
    char* key;
    int visited;
};

void
print_hashmap(const struct hashmap* map)
{
    printf("\nHashmap has %zu buckets\n", map->buckets.length);
    for (size_t i = 0; i < map->buckets.length; i++) {
        struct list_iter it = list_iter(map->buckets.elements[i]);

        printf("  %zu =>\n", i);

        list_iter_next(&it);
        while (list_iter_next(&it)) {
            struct list*          item  = list_iter_get(&it);
            struct hashmap_entry* entry = item->data;

            if (entry != NULL) {
                printf("    - %s -> %s\n",
                       (const char*)entry->key,
                       (const char*)entry->value);
            }
            else {
                printf("Dead entry -> ");
            }
        }

        printf("\n");
    }
}

struct hashmap
make_hashmap(struct entry* entries, size_t n_entries)
{
    struct hashmap map;

    hashmap_init(&map, hash_str, compare_str);
    for (size_t i = 0; i < n_entries; i++) {
        hashmap_set(&map, entries[i].key, entries[i].value);
    }

    return map;
}

void
test_insertion(void)
{
    const size_t n_entries = sizeof(large_entries) / sizeof(large_entries[0]);
    struct hashmap map     = make_hashmap(large_entries, n_entries);

    for (size_t i = 0; i < n_entries; i++) {
        struct hashmap_entry* entry
            = hashmap_lookup(&map, large_entries[i].key);

        CU_ASSERT(entry != NULL);
        CU_ASSERT(strcmp(entry->key, large_entries[i].key) == 0);
        CU_ASSERT(strcmp(entry->key, entry->value) == 0);
        CU_ASSERT(entry->alive);
    }

    hashmap_destroy(&map);
}

void
test_remove(void)
{
    const size_t n_entries = 100;
    char* remove_keys[] = {
        "tender", "forgetful", "boring", "overt", "save", "wooden", "acid"
    };
    const size_t n_remove_keys = sizeof(remove_keys) / sizeof(remove_keys[0]);
    
    struct hashmap map = make_hashmap(large_entries, n_entries);

    /* Make sure the hashmap actually contains the entries first */
    for (size_t i = 0; i < n_remove_keys; i++) {
        struct hashmap_entry* e = hashmap_lookup(&map, remove_keys[i]);
        CU_ASSERT(e != NULL);
        CU_ASSERT(strcmp(e->key, remove_keys[i]) == 0);
    }

    for (size_t i = 0; i < n_remove_keys; i++) {
        hashmap_remove(&map, remove_keys[i]);
    }

    /* Ensure the entries were actually removed */
    for (size_t i = 0; i < n_remove_keys; i++) {
        struct hashmap_entry* e = hashmap_lookup(&map, remove_keys[i]);
        CU_ASSERT(e == NULL);
    }

    hashmap_destroy(&map);
}

void
test_iter(void)
{
    const size_t n_entries  = sizeof(large_entries) / sizeof(large_entries[0]);
    struct hashmap      map = make_hashmap(large_entries, n_entries);
    struct hashmap_iter it  = hashmap_iter(&map);

    /* Counts of number of entries for each iteration method (iter,
     * manual) */
    size_t count_iter = 0;
    size_t count_manual = 0;
    
    /* used to keep track of all hashmap entries we've visited */
    struct entry_visited iter_visited[n_entries];
    struct entry_visited manual_visited[n_entries];

    for (size_t i = 0; i < n_entries; i++) {
        iter_visited[i].visited = 0;
        iter_visited[i].key = large_entries[i].key;

        manual_visited[i].visited = 0;
        manual_visited[i].key = large_entries[i].key;
    }

    /* Check the size of the hashmap using an iterator */
    while (hashmap_iter_next(&it)) {
        struct hashmap_entry* e = hashmap_iter_get(&it);
        
        count_iter++;

        for (size_t v = 0; v < n_entries; v++) {
            if (strcmp(iter_visited[v].key, e->key) == 0) {
                CU_ASSERT(!iter_visited[v].visited);
                iter_visited[v].visited = 1;
                break;
            }
        }
    }

    /* Manually kludge our way through the hashmap */
    for (size_t i = 0; i < map.buckets.length; i++) {
        struct list* l = map.buckets.elements[i];
        l              = l->next;

        while (l) {
            struct hashmap_entry* e = l->data;
            
            for (size_t v = 0; v < n_entries; v++) {
                if (strcmp(manual_visited[v].key, e->key) == 0) {
                    CU_ASSERT(!manual_visited[v].visited);
                    manual_visited[v].visited = 1;
                    break;
                }
            }

            count_manual++;
            l = l->next;
        }
    }

    /* Make sure all our size measurements agree */
    CU_ASSERT(count_iter == n_entries);
    CU_ASSERT(count_manual == n_entries);
    CU_ASSERT(count_iter == count_manual);

    /* Make sure we actually visited each item (through both iterator
     * and manual methods) */
    for (size_t i = 0; i < n_entries; i++) {
        CU_ASSERT(iter_visited[i].visited);
        CU_ASSERT(manual_visited[i].visited);
    }

    hashmap_destroy(&map);
}

void
test_iter_after_remove(void)
{
    const size_t n_entries = sizeof(large_entries) / sizeof(large_entries[0]);
    char* remove_keys[] = {
        "tender", "forgetful", "boring", "overt", "save", "wooden", "acid"
    };

    const size_t n_remove_keys = sizeof(remove_keys) / sizeof(remove_keys[0]);
    
    struct hashmap map = make_hashmap(large_entries, n_entries);

    size_t count_iter = 0;
    size_t count_iter_after_remove = 0;

    struct hashmap_iter it1 = hashmap_iter(&map);
    while (hashmap_iter_next(&it1)) {
        count_iter++;
    }

    for (size_t i = 0; i < n_remove_keys; i++) {
        hashmap_remove(&map, remove_keys[i]);
    }

    struct hashmap_iter it2 = hashmap_iter(&map);
    while (hashmap_iter_next(&it2)) {
        struct hashmap_entry* e = hashmap_iter_get(&it2);
        count_iter_after_remove++;

        for (size_t i = 0; i < n_remove_keys; i++) {
            CU_ASSERT(strcmp(remove_keys[i], e->key) != 0);
        }
    }

    CU_ASSERT(count_iter - n_remove_keys == count_iter_after_remove);
    
    hashmap_destroy(&map);
}

static struct test_case tests[] = {
    { .name = "test hashmap insertion",    .test_function = test_insertion},
    { .name = "test hashmap remove", .test_function = test_remove },
    { .name = "test hashmap iterator", .test_function = test_iter },
    { .name = "test hashmap iterator after remove", .test_function = test_iter_after_remove },
};

TEST_MAIN("hashmaps", tests)
