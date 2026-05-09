#include "unity.h"
#include "crc.h"

/* len=0: must return the initial value without touching memory */
void test_crc_empty_returns_initial_value(void)
{
    TEST_ASSERT_EQUAL_HEX16(0xFFFF, crc16_ccitt(NULL, 0));
}

/* Reference vector from CRC-16/IBM-3740 (https://reveng.sourceforge.io/crc-catalogue/16.htm#crc.cat.crc-16-ibm-3740) */
void test_crc_known_vector_123456789(void)
{
    const uint8_t data[] = {'1','2','3','4','5','6','7','8','9'};
    TEST_ASSERT_EQUAL_HEX16(0x29B1, crc16_ccitt(data, sizeof(data)));
}

/* Single 0x00 byte: manually verified */
void test_crc_single_zero_byte(void)
{
    const uint8_t data[] = {0x00};
    TEST_ASSERT_EQUAL_HEX16(0xE1F0, crc16_ccitt(data, sizeof(data)));
}

/* Same input must always yield the same result */
void test_crc_is_deterministic(void)
{
    const uint8_t data[] = {0xDE, 0xAD, 0xBE, 0xEF};
    uint16_t first  = crc16_ccitt(data, sizeof(data));
    uint16_t second = crc16_ccitt(data, sizeof(data));
    TEST_ASSERT_EQUAL_HEX16(first, second);
}

/* Flipping one bit must change the CRC */
void test_crc_different_data_yields_different_result(void)
{
    const uint8_t data_a[] = {0xAA, 0xBB, 0xCC};
    const uint8_t data_b[] = {0xAA, 0xBB, 0xCD};
    TEST_ASSERT_NOT_EQUAL(crc16_ccitt(data_a, sizeof(data_a)),
                          crc16_ccitt(data_b, sizeof(data_b)));
}

/* 1000-byte all-zero buffer (data area): result must not equal initial value */
void test_crc_large_zero_buffer_differs_from_initial(void)
{
    uint8_t data[1000] = {0};
    TEST_ASSERT_NOT_EQUAL(0xFFFF, crc16_ccitt(data, sizeof(data)));
}

void run_crc_tests(void)
{
    RUN_TEST(test_crc_empty_returns_initial_value);
    RUN_TEST(test_crc_known_vector_123456789);
    RUN_TEST(test_crc_single_zero_byte);
    RUN_TEST(test_crc_is_deterministic);
    RUN_TEST(test_crc_different_data_yields_different_result);
    RUN_TEST(test_crc_large_zero_buffer_differs_from_initial);
}
