#include "unity.h"
#include "binary.h"

void test_version_null_rejected(void)
{
	uint16_t out = 0xDEAD;
	TEST_ASSERT_EQUAL_INT(VER_ERR_NULL, parse_version(NULL, &out));
}

void test_version_empty_string_rejected(void)
{
	uint16_t out;
	TEST_ASSERT_EQUAL_INT(VER_ERR_LENGTH, parse_version("", &out));
}

/* 1 character — too short */
void test_version_one_char_rejected(void)
{
	uint16_t out;
	TEST_ASSERT_EQUAL_INT(VER_ERR_LENGTH, parse_version("9", &out));
}

/* 3 characters — too long */
void test_version_three_chars_rejected(void)
{
	uint16_t out;
	TEST_ASSERT_EQUAL_INT(VER_ERR_LENGTH, parse_version("099", &out));
}

/* First character is not a decimal digit */
void test_version_invalid_first_char_rejected(void)
{
	uint16_t out;
	TEST_ASSERT_EQUAL_INT(VER_ERR_CHAR, parse_version("A9", &out));
}

/* Second character is not a decimal digit */
void test_version_invalid_second_char_rejected(void)
{
	uint16_t out;
	TEST_ASSERT_EQUAL_INT(VER_ERR_CHAR, parse_version("9A", &out));
}

/* Minimum valid value */
void test_version_zero_decoded_correctly(void)
{
	uint16_t out = 0xDEAD;
	TEST_ASSERT_EQUAL_INT(VER_OK, parse_version("00", &out));
	TEST_ASSERT_EQUAL_UINT16(0, out);
}

/* Single-digit value with leading zero */
void test_version_leading_zero_decoded_correctly(void)
{
	uint16_t out = 0xDEAD;
	TEST_ASSERT_EQUAL_INT(VER_OK, parse_version("09", &out));
	TEST_ASSERT_EQUAL_UINT16(9, out);
}

/* Maximum valid value */
void test_version_max_decoded_correctly(void)
{
	uint16_t out = 0xDEAD;
	TEST_ASSERT_EQUAL_INT(VER_OK, parse_version("99", &out));
	TEST_ASSERT_EQUAL_UINT16(99, out);
}

/* README reference example: major=09, minor=04 */
void test_version_reference_major_decoded_correctly(void)
{
	uint16_t out = 0xDEAD;
	TEST_ASSERT_EQUAL_INT(VER_OK, parse_version("09", &out));
	TEST_ASSERT_EQUAL_UINT16(9, out);
}

void test_version_reference_minor_decoded_correctly(void)
{
	uint16_t out = 0xDEAD;
	TEST_ASSERT_EQUAL_INT(VER_OK, parse_version("04", &out));
	TEST_ASSERT_EQUAL_UINT16(4, out);
}

/* Output must remain unchanged on error */
void test_version_output_unchanged_on_null(void)
{
	uint16_t out = 0xDEAD;
	parse_version(NULL, &out);
	TEST_ASSERT_EQUAL_UINT16(0xDEAD, out);
}

void test_version_output_unchanged_on_length_error(void)
{
	uint16_t out = 0xDEAD;
	parse_version("9", &out);
	TEST_ASSERT_EQUAL_UINT16(0xDEAD, out);
}

void test_version_output_unchanged_on_char_error(void)
{
	uint16_t out = 0xDEAD;
	parse_version("9A", &out);
	TEST_ASSERT_EQUAL_UINT16(0xDEAD, out);
}

void run_version_tests(void)
{
	RUN_TEST(test_version_null_rejected);
	RUN_TEST(test_version_empty_string_rejected);
	RUN_TEST(test_version_one_char_rejected);
	RUN_TEST(test_version_three_chars_rejected);
	RUN_TEST(test_version_invalid_first_char_rejected);
	RUN_TEST(test_version_invalid_second_char_rejected);
	RUN_TEST(test_version_zero_decoded_correctly);
	RUN_TEST(test_version_leading_zero_decoded_correctly);
	RUN_TEST(test_version_max_decoded_correctly);
	RUN_TEST(test_version_reference_major_decoded_correctly);
	RUN_TEST(test_version_reference_minor_decoded_correctly);
	RUN_TEST(test_version_output_unchanged_on_null);
	RUN_TEST(test_version_output_unchanged_on_length_error);
	RUN_TEST(test_version_output_unchanged_on_char_error);
}
