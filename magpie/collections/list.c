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
