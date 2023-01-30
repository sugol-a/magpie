#ifndef MAGPIE_COLLECTION_INTEROP_H
#define MAGPIE_COLLECTION_INTEROP_H

#include <magpie/collections/array.h>
#include <magpie/collections/list.h>

int array_to_list(struct list** dest, struct array* src);
int list_to_array(struct array* dest, struct list* src);

#endif /* MAGPIE_COLLECTION_INTEROP_H */
