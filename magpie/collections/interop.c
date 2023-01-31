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

#include <magpie/collections/array.h>
#include <magpie/collections/list.h>

#define MAGPIE_INTERNAL 1
#include <magpie/ebuf.h>

#include <magpie/collections/interop.h>

int
array_to_list(struct list** dest, struct array* src)
{
    struct list* current;
    struct list_iter it;

    if (src == NULL) {
        EBUF_PUSH("source array is NULL", NULL);
        goto a2l_error;
    }

    if (src->length == 0) {
        EBUF_PUSH("source array is empty", src);
        goto a2l_error;
    }

    current = list_alloc(src->elements[0]);
    if (current == NULL) {
        EBUF_PUSH("failed to allocate first list item", NULL);
        goto a2l_error;
    }
    
    *dest = current;
    
    for (size_t i = 1; i < src->length; i++) {
        current->next = list_alloc(src->elements[i]);

        if (current->next == NULL) {
            EBUF_PUSH("failed to allocate list item", NULL);
            goto a2l_error_cleanup;
        }
        
        current = current->next;
    }

    return 1;

a2l_error_cleanup:
    it = list_iter(*dest);
    while (list_iter_next(&it)) {
        free(list_iter_get(&it));
    }
    
a2l_error:
    *dest = NULL;
    return 0;
}

int
list_to_array(struct array* dest, struct list* src)
{
    struct list_iter it = list_iter(src);

    array_clear(dest);
    while (list_iter_next(&it)) {
        array_push(dest, list_iter_get(&it)->data);
    }

    return 1;
}
