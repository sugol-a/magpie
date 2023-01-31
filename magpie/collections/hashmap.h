#ifndef MAGPIE_HASHMAP_H
#define MAGPIE_HASHMAP_H

#include <magpie/collections/list.h>
#include <magpie/collections/array.h>
#include <stdint.h>

#ifndef MAGPIE_HASHMAP_INITIAL_BUCKETS
#    define MAGPIE_HASHMAP_INITIAL_BUCKETS 128
#endif

struct hashmap {
    struct array buckets;
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
    struct array* buckets;
    size_t bucket;
    struct list_iter bucket_iter;
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
