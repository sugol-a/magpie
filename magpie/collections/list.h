#ifndef MAGPIE_LIST_H
#define MAGPIE_LIST_H

#include <stddef.h>
#include <sys/types.h>

/**
 * Linked list data structure.
 */
struct list {
    struct list* next;
    void*        data;
};

/**
 * Iterator over a linked list (see `struct list`).
 */
struct list_iter {
    struct list* current;
    struct list* pending;
    ssize_t      i;
};

/**
 * Initializes a linked list item.
 *
 * @param `l` :: Pointer to the list item.
 * @param `data` :: Data to attach to the list item
 */
void list_init(struct list* l, void* data);

/**
 * Utility function to allocate a new list item on the heap.
 *
 * @param `data` :: Data to attach to the list item.
 * @return Pointer to a list item allocated on the heap, or `NULL` if the
 * allocation failed.
 */
struct list* list_alloc(void* data);

/**
 * Traverses a linked list, returning the last item.
 *
 * @param `root` :: Pointer to the head of the list.
 * @return The last item in the list.
 */
struct list* list_last(struct list* root);

/**
 * Appends a list item to the end of a list.
 *
 * @param `root` :: Pointer to the head of the list.
 * @param `child` :: The list item to append.
 */
void list_append(struct list* root, struct list* child);

/**
 * Inserts an item into the list, just after `parent`.
 *
 * @param `parent` :: The new parent of `child`.
 * @param `child` :: The item to insert.
 */
void list_insert(struct list* parent, struct list* child);

/**
 * Removes `child` from the list pointed to by `*root`. If `*root`
 * points to the same item as `child`, the head of the list is
 * updated. A pointer to the child is returned on success, or `NULL`
 * if the child does not belong to the list.
 *
 * @param `root` :: Pointer to the head of the list.
 * @param `child` :: The item to remove from the list.
 * @return Pointer to the removed list item, or NULL.
 */
struct list* list_remove(struct list** root, struct list* child);

/**
 * Removes an item from a list, whose data equals `data` according
 * to a comparison function `comparison`.
 *
 * `compare` is a pointer to a function which should return `0` if the
 * two arguments are "equal", or non-zero otherwise.
 *
 * @param `root` :: Pointer to the head of the list.
 * @param `data` :: Data used for comparison.
 * @return Pointer to the removed list item, or NULL.
 */
struct list* list_remove_by(struct list** root,
                            void*         data,
                            int (*compare)(const void*, const void*));

/**
 * Returns an iterator over a list.
 * The list *should not* be modified when using an iterator.
 *
 * @param `root` :: Pointer to the head of the list.
 * @return An iterator over `root`.
 */
struct list_iter list_iter(struct list* root);

/**
 * Advances a list iterator.
 *
 * @param `it` :: The list iterator.
 * @return 0 if there are no more items to yield, or 1 if there are.
 */
int list_iter_next(struct list_iter* it);

/**
 * Gets the currently yielded list item from an iterator.
 *
 * @param `it` :: The list iterator.
 * @return The list item currently yielded by the iterator.
 */
struct list*     list_iter_get(struct list_iter* it);

#endif /* MAGPIE_LIST_H */
