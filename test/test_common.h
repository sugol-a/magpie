#ifndef TEST_COMMON_H
#define TEST_COMMON_H

#include <CUnit/Basic.h>
#include <CUnit/CUError.h>
#include <CUnit/TestDB.h>
#include <stddef.h>

#define TEST_MAIN(SUITE_NAME, TESTS)                                          \
    int main(void)                                                            \
    {                                                                         \
        return run_tests((SUITE_NAME), (TESTS), sizeof(TESTS) / sizeof((TESTS)[0])); \
    }

struct test_case {
    const char* name;
    void (*test_function)(void);
};

static inline int
run_tests(const char* suite_name, struct test_case* tests, size_t n_tests)
{
    CU_pSuite suite = NULL;

    if (CU_initialize_registry() != CUE_SUCCESS) {
        return CU_get_error();
    }

    suite = CU_add_suite(suite_name, NULL, NULL);

    if (suite == NULL) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    for (size_t i = 0; i < n_tests; i++) {
        if (CU_add_test(suite, tests[i].name, tests[i].test_function)
            == NULL) {
            return CU_get_error();
        }
    }

    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();

    int status = CU_get_number_of_failures();

    CU_cleanup_registry();

    return status > 0 ? 1 : 0;
}

#endif /* TEST_COMMON_H */
