#include "unity.h"
#include "binary.h"

void test_serial_null_rejected(void)
{
	uint8_t sn[5];
	TEST_ASSERT_EQUAL_INT(SN_ERR_NULL, parse_serial_number(NULL, sn));
}

void test_serial_empty_string_rejected(void)
{
	uint8_t sn[5];
	TEST_ASSERT_EQUAL_INT(SN_ERR_LENGTH, parse_serial_number("", sn));
}

/* 6 characters — one too short */
void test_serial_too_short_rejected(void)
{
	uint8_t sn[5];
	TEST_ASSERT_EQUAL_INT(SN_ERR_LENGTH, parse_serial_number("X55000", sn));
}

/* 8 characters — one too long */
void test_serial_too_long_rejected(void)
{
	uint8_t sn[5];
	TEST_ASSERT_EQUAL_INT(SN_ERR_LENGTH, parse_serial_number("X5500081", sn));
}

/* '0' is not a valid year character */
void test_serial_invalid_year_rejected(void)
{
	uint8_t sn[5];
	TEST_ASSERT_EQUAL_INT(SN_ERR_YEAR, parse_serial_number("0550008", sn));
}

/* 'D' is not a valid month character (valid: 1-9, A-C) */
void test_serial_invalid_month_rejected(void)
{
	uint8_t sn[5];
	TEST_ASSERT_EQUAL_INT(SN_ERR_MONTH, parse_serial_number("XD50008", sn));
}

/* 'B' is not a valid device type character (valid: 4-9, A) */
void test_serial_invalid_type_rejected(void)
{
	uint8_t sn[5];
	TEST_ASSERT_EQUAL_INT(SN_ERR_TYPE, parse_serial_number("X5B0008", sn));
}

/* 'A' in sequence field is not a decimal digit */
void test_serial_invalid_seq_rejected(void)
{
	uint8_t sn[5];
	TEST_ASSERT_EQUAL_INT(SN_ERR_SEQ, parse_serial_number("X550A08", sn));
}

/* Valid serial number must be accepted */
void test_serial_valid_accepted(void)
{
	uint8_t sn[5];
	TEST_ASSERT_EQUAL_INT(SN_OK, parse_serial_number("X550008", sn));
}

/* README example: X550008 → year=2009(9), month=May(5), type=Typ2(2), seq=8 */
void test_serial_reference_example_decoded_correctly(void)
{
	uint8_t sn[5];
	const uint8_t expected[5] = {9, 5, 2, 0, 8};
	TEST_ASSERT_EQUAL_INT(SN_OK, parse_serial_number("X550008", sn));
	TEST_ASSERT_EQUAL_HEX8_ARRAY(expected, sn, 5);
}

/* AA41234 → year=2010(10), month=Oct(10), type=Typ1(1), seq=1234(0x04D2) */
void test_serial_aa41234_decoded_correctly(void)
{
	uint8_t sn[5];
	const uint8_t expected[5] = {10, 10, 1, 0x04, 0xD2};
	TEST_ASSERT_EQUAL_INT(SN_OK, parse_serial_number("AA41234", sn));
	TEST_ASSERT_EQUAL_HEX8_ARRAY(expected, sn, 5);
}

/* Sequence number 9999 — maximum valid value */
void test_serial_max_seq_decoded_correctly(void)
{
	uint8_t sn[5];
	const uint8_t expected[5] = {9, 5, 2, 0x27, 0x0F};
	TEST_ASSERT_EQUAL_INT(SN_OK, parse_serial_number("X559999", sn));
	TEST_ASSERT_EQUAL_HEX8_ARRAY(expected, sn, 5);
}

/* On failure the output buffer must remain unchanged */
void test_serial_output_unchanged_on_error(void)
{
	uint8_t sn[5] = {0xDE, 0xAD, 0xBE, 0xEF, 0xCA};
	const uint8_t sentinel[5] = {0xDE, 0xAD, 0xBE, 0xEF, 0xCA};
	parse_serial_number("0550008", sn);
	TEST_ASSERT_EQUAL_HEX8_ARRAY(sentinel, sn, 5);
}

void run_serial_number_tests(void)
{
	RUN_TEST(test_serial_null_rejected);
	RUN_TEST(test_serial_empty_string_rejected);
	RUN_TEST(test_serial_too_short_rejected);
	RUN_TEST(test_serial_too_long_rejected);
	RUN_TEST(test_serial_invalid_year_rejected);
	RUN_TEST(test_serial_invalid_month_rejected);
	RUN_TEST(test_serial_invalid_type_rejected);
	RUN_TEST(test_serial_invalid_seq_rejected);
	RUN_TEST(test_serial_valid_accepted);
	RUN_TEST(test_serial_reference_example_decoded_correctly);
	RUN_TEST(test_serial_aa41234_decoded_correctly);
	RUN_TEST(test_serial_max_seq_decoded_correctly);
	RUN_TEST(test_serial_output_unchanged_on_error);
}
