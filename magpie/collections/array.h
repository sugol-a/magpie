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

#ifndef MAGPIE_ARRAY_H
#define MAGPIE_ARRAY_H

#include <stddef.h>
#include <sys/types.h>

#ifndef MAGPIE_DEFAULT_ARRAY_CAPACITY
#    define MAGPIE_DEFAULT_ARRAY_CAPACITY 64
#endif

/**
 * A generic array structure.
 *
 * - `elements` :: Contains the array elements
 * - `capacity` :: Size of the memory allocated for elements (units of
 * `sizeof(*elements)`)
 * - `length`   :: Number of elements in the array
 */
struct array {
    void** elements;
    size_t capacity;
    size_t length;
};

/**
 * Specifies the algorithm to use for sorting an array. Used in
 * `array_sort()`.
 *
 * - `ARRAY_QUICKSORT` :: Unstable sorting implementation (internally
 *   uses the standard `qsort()` function).
 * - `ARRAY_INSERTION_SORT` :: Stable sorting implementation.
 */
enum array_sort_algorithm {
    ARRAY_QUICKSORT = 0,
    ARRAY_INSERTION_SORT,
};

/**
 * Specifies whether to sort an array in ascending or descending
 * order.
 *
 * - `ARRAY_SORT_ASCENDING` :: Sort an array in ascending order
 * - `ARRAY_SORT_DESCENDING` :: Sort an array in descending order
 */
enum array_sort_direction {
    ARRAY_SORT_ASCENDING = 0,
    ARRAY_SORT_DESCENDING,
};

/**
 * Initializes an array with the default capacity
 * (`MAGPIE_DEFAULT_ARRAY_CAPACITY`).
 *
 * @param `a` :: Pointer to the array
 * @return 0 on error
 */
int array_init(struct array* a);

/**
 * Initializes an array using the specified capacity.
 *
 * @param `a` :: Pointer to the array.
 * @param `capacity` :: Amount of memory to allocate initially (units of
 * `sizeof(*elements)`).
 * @return 0 on error.
 */
int array_init_with_capacity(struct array* a, size_t capacity);

/**
 * Deallocates an array.
 *
 * @param `a` :: Pointer to the array.
 */
void array_destroy(struct array* a);

/**
 * Pushes an element to the end of an array, reallocating as
 * necessary.
 *
 * @param `a` :: Pointer to the array.
 * @param `element` :: Element to push onto the array.
 */
void array_push(struct array* a, void* element);

/**
 * Pops an element from the end of an array, storing the popped
 * element in `*element`.
 *
 * @param `a` :: Pointer to the array.
 * @param `element` :: Pointer to a `void*` to store the popped element into.
 * Can be `NULL`.
 * @return 0 if the array is empty.
 */
int array_pop(struct array* a, void** element);

/**
 * Insert an element at the specified index inside an array,
 * reallocating as necessary.
 *
 * @param `a` :: Pointer to the array.
 * @param `index` :: Index to insert the element at.
 * @param `element` :: The element to insert.
 */
void array_insert(struct array* a, size_t index, void* element);

/**
 * Insert an element in sorted order into an array. The array must
 * already be sorted in ascending order.
 *
 * `compare` is a pointer to a function which compares two
 * elements. The function should return
 *
 * - A negative integer if the first argument is less than the second
 * - A positive integer if the first argument is greater than the second
 * - Zero if the two arguments are equal
 *
 * @param `a` :: Pointer to the array.
 * @param `compare` :: Function for comparing two elements.
 * @param `element` :: The element to insert.
 */
void array_insert_sorted(struct array* a,
                         int (*compare)(const void*, const void*),
                         void* element);

/**
 * Removes the element at the specified index in an array and stores
 * that element inside `*element`.
 *
 * @param `a` :: Pointer to the array.
 * @param `index` :: Index of the element to remove.
 * @param `element` :: Pointer to a `void*` to store the removed element into.
 * Can be `NULL`.
 * @return 0 on error.
 */
int array_remove(struct array* a, size_t index, void** element);

/**
 * Clears an array without deallocating it.
 *
 * @param `a` :: Array to clear.
 */
void array_clear(struct array* a);

/**
 * Sorts an array in-place using the specified algorithm and ordering.
 *
 * `compare` is a pointer to a function which compares two
 * elements. The function should return
 *
 * - A negative integer if the first argument is less than the second
 * - A positive integer if the first argument is greater than the second
 * - Zero if the two arguments are equal
 *
 * @param `a` :: Pointer to the array
 * @param `compare` :: Pointer to a function used for comparing two elements.
 * @param `algorithm` :: Algorithm to use for sorting. See `enum
 * array_sort_algorithm`.
 * @param `direction` :: Ordering to use for array elements. See `enum
 * array_sort_direction`.
 */
void array_sort(struct array* a,
                int (*compare)(const void*, const void*),
                int algorithm,
                int direction);

/**
 * Finds the first element in an array which equals `element`.
 * Pointer equality is used for this function, use `array_find_by()`
 * for value equality.
 *
 * @param `a` :: Pointer to the array.
 * @param `element` :: Element to search for.
 * @return Index of the element, or -1 if the element is not found.
 */
ssize_t array_find(struct array* a, const void* element);

/**
 * Finds the first element in an array which equals `element`, using
 * value equality.
 *
 * `compare` is a pointer to a function which compares two
 * elements. It should return 0 if the two arguments are equal, and
 * non-zero otherwise.
 *
 * @param `a` :: Pointer to the array.
 * @param `compare` :: Pointer to a comparison function.
 * @param `element` :: Element to search for.
 * @return Index of the element, or -1 if the element is not found.
 */
ssize_t array_find_by(struct array* a,
                      int (*compare)(const void*, const void*),
                      const void* element);

/**
 * Finds the index of `element` in the array using a binary
 * search. The array should be sorted before calling this function;
 * see `array_sort()`.
 *
 * `compare` is a pointer to a function which compares two elements. It should
 * return
 *  - A negative integer if the first argument is less than the second
 *  - A positive integer if the first argument is greater than the second
 *  - Zero if the two arguments are equal
 *
 * @param `a` :: Pointer to the array.
 * @param `compare` :: Pointer to a comparison function.
 * @param `element` :: Element to search for.
 */
ssize_t array_binary_search(struct array* a,
                            int (*compare)(const void*, const void*),
                            const void* element);

#endif /* MAGPIE_ARRAY_H */
