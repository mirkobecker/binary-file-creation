#include "unity.h"

void run_crc_tests(void);

void setUp(void) {}
void tearDown(void) {}

int main(void)
{
    UNITY_BEGIN();
    run_crc_tests();
    return UNITY_END();
}
