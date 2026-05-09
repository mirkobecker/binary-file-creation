#include "unity.h"
#include "binary.h"

void test_mac_null_rejected(void)
{
    uint8_t mac[6];
    TEST_ASSERT_EQUAL_INT(MAC_ERR_NULL, parse_mac(NULL, mac));
}

void test_mac_empty_string_rejected(void)
{
    uint8_t mac[6];
    TEST_ASSERT_EQUAL_INT(MAC_ERR_LENGTH, parse_mac("", mac));
}

/* 11 characters — one too short */
void test_mac_too_short_rejected(void)
{
    uint8_t mac[6];
    TEST_ASSERT_EQUAL_INT(MAC_ERR_LENGTH, parse_mac("00157E33AAF", mac));
}

/* 13 characters — one too long */
void test_mac_too_long_rejected(void)
{
    uint8_t mac[6];
    TEST_ASSERT_EQUAL_INT(MAC_ERR_LENGTH, parse_mac("00157E33AAFF0", mac));
}

/* Lowercase hex must be rejected — spec allows only [0-9A-F] */
void test_mac_lowercase_rejected(void)
{
    uint8_t mac[6];
    TEST_ASSERT_EQUAL_INT(MAC_ERR_CHAR, parse_mac("00157e33aaff", mac));
}

/* Non-hex character ('G') must be rejected */
void test_mac_invalid_char_rejected(void)
{
    uint8_t mac[6];
    TEST_ASSERT_EQUAL_INT(MAC_ERR_CHAR, parse_mac("00157G33AAFF", mac));
}

/* Broadcast address FFFFFFFFFFFF must be rejected */
void test_mac_broadcast_rejected(void)
{
    uint8_t mac[6];
    TEST_ASSERT_EQUAL_INT(MAC_ERR_BROADCAST, parse_mac("FFFFFFFFFFFF", mac));
}

/* Valid MAC must be accepted */
void test_mac_valid_accepted(void)
{
    uint8_t mac[6];
    TEST_ASSERT_EQUAL_INT(MAC_OK, parse_mac("00157E33AAFF", mac));
}

/* Valid MAC must be decoded into the correct bytes */
void test_mac_bytes_decoded_correctly(void)
{
    uint8_t mac[6];
    const uint8_t expected[6] = {0x00, 0x15, 0x7E, 0x33, 0xAA, 0xFF};
    TEST_ASSERT_EQUAL_INT(MAC_OK, parse_mac("00157E33AAFF", mac));
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected, mac, 6);
}

/* Second reference MAC from the example */
void test_mac_second_reference_mac_decoded_correctly(void)
{
    uint8_t mac[6];
    const uint8_t expected[6] = {0x00, 0x15, 0x7E, 0x33, 0xAB, 0x00};
    TEST_ASSERT_EQUAL_INT(MAC_OK, parse_mac("00157E33AB00", mac));
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected, mac, 6);
}

/* On failure the output buffer must remain unchanged */
void test_mac_output_unchanged_on_error(void)
{
    uint8_t mac[6] = {0xDE, 0xAD, 0xBE, 0xEF, 0xCA, 0xFE};
    const uint8_t sentinel[6] = {0xDE, 0xAD, 0xBE, 0xEF, 0xCA, 0xFE};
    parse_mac("FFFFFFFFFFFF", mac);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(sentinel, mac, 6);
}

void run_binary_tests(void)
{
    RUN_TEST(test_mac_null_rejected);
    RUN_TEST(test_mac_empty_string_rejected);
    RUN_TEST(test_mac_too_short_rejected);
    RUN_TEST(test_mac_too_long_rejected);
    RUN_TEST(test_mac_lowercase_rejected);
    RUN_TEST(test_mac_invalid_char_rejected);
    RUN_TEST(test_mac_broadcast_rejected);
    RUN_TEST(test_mac_valid_accepted);
    RUN_TEST(test_mac_bytes_decoded_correctly);
    RUN_TEST(test_mac_second_reference_mac_decoded_correctly);
    RUN_TEST(test_mac_output_unchanged_on_error);
}
