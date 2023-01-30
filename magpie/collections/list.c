#include <stdlib.h>

#define MAGPIE_INTERNAL 1
#include <magpie/collections/list.h>
#include <magpie/ebuf.h>

void
list_init(struct list* l, void* data)
{
    l->next = NULL;
    l->data = data;
}

struct list*
list_alloc(void* data)
{
    struct list* l = malloc(sizeof(*l));

    if (l == NULL) {
        EBUF_PUSH("failed to alloc list", NULL);
        return NULL;
    }

    list_init(l, data);
    return l;
}

struct list*
list_last(struct list* root)
{
    struct list* l = root;

    while (l->next != NULL) {
        l = l->next;
    }

    return l;
}

void
list_append(struct list* root, struct list* child)
{
    struct list* last = list_last(root);

    if (last == NULL) {
        return;
    }

    last->next  = child;
    child->next = NULL;
}

void
list_insert(struct list* parent, struct list* child)
{
    if (parent == NULL || child == NULL) {
        return;
    }
    
    child->next  = parent->next;
    parent->next = child;
}

struct list*
list_remove(struct list** root, struct list* child)
{
    struct list_iter it;

    if (*root == NULL || child == NULL) {
        return NULL;
    }

    /* Edge case: if the root is the item we're looking for, we need
     * to change the root pointer */
    if (*root == child) {
        struct list* ret = *root;
        *root            = (*root)->next;

        return ret;
    }

    it = list_iter(*root);
    while (list_iter_next(&it)) {
        if (it.pending == child) {
            /* unlink the list item and return it */
            it.current->next = it.pending->next;
            return it.pending;
        }
    }

    return NULL;
}

struct list*
list_remove_by(struct list** root,
               void*         data,
               int (*compare)(const void*, const void*))
{
    struct list* l;

    if (*root == NULL) {
        return NULL;
    }

    if (compare(&(*root)->data, &data) == 0) {
        struct list* ret = *root;
        *root            = (*root)->next;

        return ret;
    }

    l = *root;
    while (l != NULL) {
        if (l->next && compare(&l->next->data, &data) == 0) {
            struct list* ret = l->next;
            l->next = l->next->next;

            return ret;
        }
        
        l = l->next;
    }

    return NULL;
}

struct list_iter
list_iter(struct list* root)
{
    struct list_iter it = {
        .current = NULL,
        .pending = root,
        .i       = -1,
    };

    return it;
}

int
list_iter_next(struct list_iter* it)
{
    it->current = it->pending;
    it->i++;
    
    if (it->current == NULL) {
        /* nothing left; don't advance */
        return 0;
    }

    it->pending = it->pending->next;
    return 1;
}

struct list*
list_iter_get(struct list_iter* it)
{
    return it->current;
}
