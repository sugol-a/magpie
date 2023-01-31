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
