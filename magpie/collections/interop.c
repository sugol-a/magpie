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
