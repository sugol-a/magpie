#include <CUnit/Basic.h>
#include <CUnit/CUnit.h>
#include <stdlib.h>

#include "test_common.h"
#include <magpie/collections/array.h>

struct array
make_random_array(size_t length)
{
    struct array a;
    array_init_with_capacity(&a, length);

    srand(0);
    for (size_t i = 0; i < length; i++) {
        array_push(&a, (void*)((ssize_t)rand() % 100));
    }

    return a;
}

int
compare_int(const void* a, const void* b)
{
    int x = *(ssize_t*)a;
    int y = *(ssize_t*)b;

    return x < y ? -1 : x > y ? 1 : 0;
}

int
is_sorted(const struct array* a, int descending)
{
    for (size_t i = 0; i < a->length - 1; i++) {
        int x  = (ssize_t)a->elements[i];
        int xn = (ssize_t)a->elements[i + 1];

        if ((x > xn && !descending) || (x < xn && descending)) {
            return 0;
        }
    }

    return 1;
}

void
test_insertion_sort_ascending(void)
{
    struct array a = make_random_array(1000);

    array_sort(&a, compare_int, ARRAY_INSERTION_SORT, ARRAY_SORT_ASCENDING);
    CU_ASSERT(is_sorted(&a, 0));
    array_destroy(&a);
}

void
test_insertion_sort_descending(void)
{
    struct array a = make_random_array(1000);

    array_sort(&a, compare_int, ARRAY_INSERTION_SORT, ARRAY_SORT_DESCENDING);
    CU_ASSERT(is_sorted(&a, 1));
    array_destroy(&a);
}

void
test_quicksort_ascending(void)
{
    struct array a = make_random_array(1000);

    array_sort(&a, compare_int, ARRAY_QUICKSORT, ARRAY_SORT_ASCENDING);
    CU_ASSERT(is_sorted(&a, 0));
    array_destroy(&a);
}

void
test_quicksort_descending(void)
{
    struct array a = make_random_array(1000);

    array_sort(&a, compare_int, ARRAY_QUICKSORT, ARRAY_SORT_DESCENDING);
    CU_ASSERT(is_sorted(&a, 1));
    array_destroy(&a);
}

void
test_push(void)
{
    struct array a;
    size_t       values[]   = { 4, 8, 15, 16, 23, 42 };
    const size_t values_len = sizeof(values) / sizeof(values[0]);

    array_init_with_capacity(&a, values_len);

    for (size_t i = 0; i < values_len; i++) {
        array_push(&a, (void*)values[i]);
    }

    CU_ASSERT(a.length == values_len);

    for (size_t i = 0; i < a.length; i++) {
        size_t value = (size_t)a.elements[i];

        CU_ASSERT(value == values[i]);
    }

    array_destroy(&a);
}

void
test_pop(void)
{
    struct array a;
    size_t       values[]   = { 4, 8, 15, 16, 23, 42 };
    const size_t values_len = sizeof(values) / sizeof(values[0]);

    array_init_with_capacity(&a, values_len);

    for (size_t i = 0; i < values_len; i++) {
        array_push(&a, (void*)values[i]);
    }

    CU_ASSERT(a.length == values_len);

    size_t value;
    size_t i = 0;
    while (array_pop(&a, (void**)&value)) {
        CU_ASSERT(value == values[values_len - i - 1]);
        i++;
    }

    CU_ASSERT(a.length == 0);

    array_destroy(&a);
}

void
test_binary_search(void)
{
    struct array a;
    const size_t n_values = 1000;
    array_init_with_capacity(&a, n_values);

    for (size_t i = 0; i < n_values; i++) {
        array_push(&a, (void*)(2 * i));
    }

    CU_ASSERT(a.length == n_values);

    for (size_t i = 0; i < n_values; i++) {
        ssize_t index;

        /* search for values we've definitely pushed to the array */
        index = array_binary_search(&a, compare_int, (void*)(2 * i));
        CU_ASSERT(index == (ssize_t)i);

        /* the array only contains even numbers; search for an odd
         * number & ensure failure */
        index = array_binary_search(&a, compare_int, (void*)(2 * i + 1));
        CU_ASSERT(index == -1);

        /* search for even numbers we definitely didn't push to the
         * array & ensure failure */
        index = array_binary_search(&a,
                                    compare_int,
                                    (void*)(2 * (i + n_values)));
        CU_ASSERT(index == -1);
    }

    array_destroy(&a);
}

void
test_insert(void)
{
    struct array a;
    const size_t n_initial_elements = 100;
    array_init_with_capacity(&a, 1);

    for (size_t i = 0; i < n_initial_elements; i++) {
        array_push(&a, (void*)(2 * i));
    }

    CU_ASSERT(a.length == n_initial_elements);

    /* Ensure that items get inserted at the exact index we specify */
    array_insert(&a, 50, (void*)3);
    CU_ASSERT(array_find(&a, (void*)3) == 50);
    CU_ASSERT(a.length == n_initial_elements + 1);

    /* Ensure that we don't overwrite any other items */
    array_insert(&a, 50, (void*)5);
    CU_ASSERT(array_find(&a, (void*)5) == 50);
    CU_ASSERT(array_find(&a, (void*)3) == 51);
    CU_ASSERT(a.length == n_initial_elements + 2);

    array_destroy(&a);
}

void
test_remove(void)
{
    struct array a;
    const size_t n_initial_elements = 100;
    size_t       element;
    array_init(&a);

    for (size_t i = 0; i < n_initial_elements; i++) {
        array_push(&a, (void*)i);
    }

    CU_ASSERT(a.length == n_initial_elements);

    CU_ASSERT(array_find(&a, (void*)50) == 50);
    array_remove(&a, 50, (void**)&element);
    CU_ASSERT(element == 50);
    CU_ASSERT(a.length == n_initial_elements - 1);
    CU_ASSERT(array_find(&a, (void*)50) == -1);

    array_destroy(&a);
}

void
test_insert_sorted(void)
{
    struct array a      = make_random_array(100);
    struct array values = make_random_array(50);

    array_sort(&a, compare_int, ARRAY_QUICKSORT, ARRAY_SORT_ASCENDING);

    for (size_t i = 0; i < values.length; i++) {
        array_insert_sorted(&a, compare_int, values.elements[i]);
    }

    CU_ASSERT(is_sorted(&a, 0));
}

static struct test_case tests[] = {
    {.name          = "test insertion sort (ascending)",
     .test_function = test_insertion_sort_ascending                                           },

    { .name          = "test insertion sort (descending)",
     .test_function = test_insertion_sort_descending                                          },

    { .name          = "test quicksort (ascending)",
     .test_function = test_quicksort_ascending                                                },

    { .name          = "test quicksort (descending)",
     .test_function = test_quicksort_descending                                               },

    { .name = "test push",                                 .test_function = test_push         },

    { .name = "test pop",                                  .test_function = test_pop          },

    { .name = "test binary search",                        .test_function = test_binary_search},

    { .name = "test insert",                               .test_function = test_insert       },

    { .name = "test remove",                               .test_function = test_remove       },

    { .name = "test insert sorted",                        .test_function = test_insert_sorted},
};

TEST_MAIN("Arrays", tests)
