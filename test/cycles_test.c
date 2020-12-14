#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>
#include "cycles.h"

void test_create_clock_cycle()
{
    Cycle *cycle = create_cycle(CLOCK_CYCLE);

    uint32_t chunk = MS_CONVERSION / CPU_RATE;
    CU_ASSERT_EQUAL(cycle->type, CLOCK_CYCLE);
    CU_ASSERT_EQUAL(cycle->chunk, chunk);
    CU_ASSERT_EQUAL(cycle->start, 0);
    CU_ASSERT_EQUAL(cycle->elapsed, 0);

    destroy_cycle(cycle);
}

void test_create_timer_cycle()
{
    Cycle *cycle = create_cycle(TIMER_CYCLE);

    uint32_t chunk = MS_CONVERSION / DELAY_RATE;
    CU_ASSERT_EQUAL(cycle->type, TIMER_CYCLE);
    CU_ASSERT_EQUAL(cycle->chunk, chunk);
    CU_ASSERT_EQUAL(cycle->start, 0);
    CU_ASSERT_EQUAL(cycle->elapsed, 0);

    destroy_cycle(cycle);
}

// Probably fails because it calls SDL
void test_reset_cycle()
{
    Cycle *cycle = create_cycle(CLOCK_CYCLE);
    CU_ASSERT_EQUAL(cycle->start, 0);

    reset_cycle(cycle);
    CU_ASSERT_NOT_EQUAL(cycle->start, 0);

    destroy_cycle(cycle);
}

int main(void)
{
    if (CU_initialize_registry() != CUE_SUCCESS) {
        printf("Failed to initialize registry.\n");
        return CU_get_error();
    }

    CU_Suite *test_suite = CU_add_suite("Cycle unit tests", NULL, NULL);
    if (test_suite == NULL) {
        printf("Failed to add suite to registry.\n");
        CU_cleanup_registry();
        return CU_get_error();
    }

    if (CU_add_test(
            test_suite,
            "Cycles | create_cycle initializes clock cycle",
            test_create_clock_cycle) == NULL ||
        CU_add_test(
            test_suite,
            "Cycles | create_cycle initializes timer cycle",
            test_create_timer_cycle) == NULL) {
        printf("Failed to add tests to suite.\n");
        CU_cleanup_registry();
        return CU_get_error();
    }

    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();
    CU_cleanup_registry();

    return CU_get_error();
}
