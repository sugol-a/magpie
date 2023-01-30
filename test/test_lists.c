#include <stdlib.h>

#include "test_common.h"
#include <CUnit/Basic.h>
#include <magpie/collections/list.h>
#include <string.h>

char* strings[] = { "Hello", "world", "this", "is", "a", "test" };

int
compare_strings(const void* a, const void* b)
{
    const char* string_a = *(const char**)a;
    const char* string_b = *(const char**)b;

    return strcmp(string_a, string_b);
}

int
compare_startswith(const void* a, const void* b)
{
    const char* string_a = *(const char**)a;
    const char* string_b = *(const char**)b;

    size_t b_len = strlen(string_b);

    int r = strncmp(string_a, string_b, b_len);
    return r;
}

struct list*
make_list(void)
{
    struct list* root = list_alloc(strings[0]);
    for (size_t i = 1; i < sizeof(strings) / sizeof(strings[0]); i++) {
        list_append(root, list_alloc(strings[i]));
    }

    return root;
}

void
free_list(struct list* root)
{
    struct list_iter it = list_iter(root);

    while (list_iter_next(&it)) {
        struct list* l = list_iter_get(&it);
        free(l);
    }
}

void
test_append(void)
{
    struct list*     root = make_list();
    struct list_iter it = list_iter(root);
    
    while (list_iter_next(&it)) {
        const struct list* l = list_iter_get(&it);
        CU_ASSERT(l->data == strings[it.i]);
    }

    CU_ASSERT(it.i == sizeof(strings) / sizeof(strings[0]));
    free_list(root);
}

void
test_remove_first(void)
{
    struct list* root = make_list();

    struct list* test_root = root;
    struct list* removed = list_remove(&test_root, root);

    /* since we're removing the first item from the list, test_root
     * should be updated */
    CU_ASSERT(removed == root);
    CU_ASSERT(test_root == root->next);
    
    free_list(test_root);
    free(removed);
}

void
test_remove_mid(void)
{
    struct list* root = make_list();

    /* remove the second item from the list. the list should be
     * updated such that the third item becomes the second. */
    struct list* third = root->next->next;
    struct list* removed = list_remove(&root, root->next);

    CU_ASSERT(root->next == third);

    free_list(root);
    free(removed);
}

void
test_insert(void)
{
    struct list* root = make_list();
    struct list* old_next = root->next;
    struct list* new_next = list_alloc("inserted");
    list_insert(root, new_next);

    CU_ASSERT(root->next == new_next);
    CU_ASSERT(root->next->next == old_next);

    free_list(root);
}

void
test_remove_by_first(void)
{
    struct list* root = make_list();

    struct list* test_root = root;
    struct list* removed = list_remove_by(&test_root, "Hello", compare_strings);

    /* since we're removing the first item from the list, test_root
     * should be updated */
    CU_ASSERT(removed == root);
    CU_ASSERT(test_root == root->next);
    
    free_list(test_root);
    free(removed);
}

void
test_remove_by_mid(void)
{
    struct list* root = make_list();

    /* remove the second item from the list. the list should be
     * updated such that the third item becomes the second. */
    struct list* third = root->next->next;
    struct list* removed = list_remove_by(&root, "world", compare_strings);

    CU_ASSERT(root->next == third);

    free_list(root);
    free(removed);
}

void
test_remove_multi(void)
{
    char* removeme_strings[] = {
        "RemoveMe Hello",
        "RemoveMe World",
        "RemoveMe This",
        "Is",
        "A",
        "Test",
        "RemoveMe ."
    };
    
    struct list* root = list_alloc(removeme_strings[0]);
    for (size_t i = 1; i < sizeof(removeme_strings) / sizeof(removeme_strings[0]); i++) {
        list_append(root, list_alloc(removeme_strings[i]));
    }

    /* Remove all items whose string starts with "RemoveMe" */
    struct list* l;
    while ((l = list_remove_by(&root, "RemoveMe", compare_startswith)) != NULL) {
        /* Make sure the removed item starts with "RemoveMe" */
        CU_ASSERT(strncmp(l->data, "RemoveMe", strlen("RemoveMe")) == 0);
        free(l);
    }

    struct list_iter it = list_iter(root);
    while (list_iter_next(&it)) {
        struct list* l = list_iter_get(&it);

        /* Make sure remaining items do not start with "RemoveMe" */
        CU_ASSERT(strncmp(l->data, "RemoveMe", strlen("RemoveMe")) != 0);
    }

    CU_ASSERT(it.i == 3);
    free_list(root);
}

static struct test_case tests[] = {
    {.name = "test append", .test_function = test_append},
    {.name = "test remove first", .test_function = test_remove_first},
    {.name = "test remove mid", .test_function = test_remove_mid},
    {.name = "test insert", .test_function = test_insert },
    {.name = "test remove_by first", .test_function = test_remove_by_first },
    {.name = "test remove multiple", .test_function = test_remove_multi }
};

TEST_MAIN("linked lists", tests)
