#include "unity.h"
#include "binary.h"
#include "crc.h"
#include <string.h>

/* Absolute header offsets into the 1024-byte buffer */
#define OFFSET_HEADER_HCHK 0x00
#define OFFSET_HEADER_SIG  0x02
#define OFFSET_HEADER_DCHK 0x08
#define OFFSET_HEADER_SIZE 0x10
#define OFFSET_HEADER_DATA 0x18

/* Offsets within the data area (relative to OFFSET_HEADER_DATA) */
#define OFFSET_DATA_MAC1  0x00
#define OFFSET_DATA_MAC2  0x06
#define OFFSET_DATA_MAJOR 0x0C
#define OFFSET_DATA_MINOR 0x0E
#define OFFSET_DATA_SN    0x10
#define OFFSET_DATA_RES   0x15

#define SIZE_HEADER      24
#define SIZE_DATA        1000
#define SIZE_TOTAL       1024
#define SIZE_MAC         6
#define SIZE_SN          5
#define VALID_DATA_BYTES 21

/* Reference inputs from README example: create_binary_file 00157E33AAFF 00157E33AB00 X550008 09 04 */
static const uint8_t  REF_MAC1[6] = {0x00, 0x15, 0x7E, 0x33, 0xAA, 0xFF};
static const uint8_t  REF_MAC2[6] = {0x00, 0x15, 0x7E, 0x33, 0xAB, 0x00};
static const uint16_t REF_MAJOR   = 9;
static const uint16_t REF_MINOR   = 4;
/* X550008 → year=9, month=5, type=2, seq=8 */
static const uint8_t REF_SN[5] = {9, 5, 2, 0, 8};

static uint8_t buf[SIZE_TOTAL];

static void build_reference(void)
{
	build_binary(REF_MAC1, REF_MAC2, REF_MAJOR, REF_MINOR, REF_SN, buf);
}

static uint16_t read_u16_be(const uint8_t *p)
{
	return (uint16_t)((p[0] << 8) | p[1]);
}

/* ---- Data area field placement ---- */

void test_build_binary_mac1_placed_correctly(void)
{
	build_reference();
	TEST_ASSERT_EQUAL_HEX8_ARRAY(REF_MAC1, buf + OFFSET_HEADER_DATA + OFFSET_DATA_MAC1, SIZE_MAC);
}

void test_build_binary_mac2_placed_correctly(void)
{
	build_reference();
	TEST_ASSERT_EQUAL_HEX8_ARRAY(REF_MAC2, buf + OFFSET_HEADER_DATA + OFFSET_DATA_MAC2, SIZE_MAC);
}

void test_build_binary_major_stored_big_endian(void)
{
	build_reference();
	uint16_t major = read_u16_be(buf + OFFSET_HEADER_DATA + OFFSET_DATA_MAJOR);
	TEST_ASSERT_EQUAL_UINT16(REF_MAJOR, major);
	/* High byte must be at the lower address */
	TEST_ASSERT_EQUAL_HEX8(0x00, buf[OFFSET_HEADER_DATA + OFFSET_DATA_MAJOR]);
	TEST_ASSERT_EQUAL_HEX8(0x09, buf[OFFSET_HEADER_DATA + OFFSET_DATA_MAJOR + 1]);
}

void test_build_binary_minor_stored_big_endian(void)
{
	build_reference();
	uint16_t minor = read_u16_be(buf + OFFSET_HEADER_DATA + OFFSET_DATA_MINOR);
	TEST_ASSERT_EQUAL_UINT16(REF_MINOR, minor);
	/* High byte must be at the lower address */
	TEST_ASSERT_EQUAL_HEX8(0x00, buf[OFFSET_HEADER_DATA + OFFSET_DATA_MINOR]);
	TEST_ASSERT_EQUAL_HEX8(0x04, buf[OFFSET_HEADER_DATA + OFFSET_DATA_MINOR + 1]);
}

void test_build_binary_sn_placed_correctly(void)
{
	build_reference();
	TEST_ASSERT_EQUAL_HEX8_ARRAY(REF_SN, buf + OFFSET_HEADER_DATA + OFFSET_DATA_SN, SIZE_SN);
}

void test_build_binary_data_reserved_zeroed(void)
{
	build_reference();
	for (int i = OFFSET_HEADER_DATA + OFFSET_DATA_RES; i < SIZE_TOTAL; i++) {
		if (buf[i] != 0x00) {
			char msg[40];
			snprintf(msg, sizeof(msg), "byte 0x%03X not zero", i);
			TEST_FAIL_MESSAGE(msg);
		}
	}
}

/* ---- Header field placement ---- */

void test_build_binary_signature_correct(void)
{
	build_reference();
	TEST_ASSERT_EQUAL_HEX8('W', buf[OFFSET_HEADER_SIG + 0]);
	TEST_ASSERT_EQUAL_HEX8('R', buf[OFFSET_HEADER_SIG + 1]);
	TEST_ASSERT_EQUAL_HEX8('E', buf[OFFSET_HEADER_SIG + 2]);
	TEST_ASSERT_EQUAL_HEX8('C', buf[OFFSET_HEADER_SIG + 3]);
}

void test_build_binary_header_reserved_zeroed(void)
{
	build_reference();
	/* Reserved between SIG and DCHK */
	for (int i = OFFSET_HEADER_SIG + 4; i < OFFSET_HEADER_DCHK; i++) {
		TEST_ASSERT_EQUAL_HEX8(0x00, buf[i]);
	}
	/* Reserved between DCHK and SIZE */
	for (int i = OFFSET_HEADER_DCHK + 2; i < OFFSET_HEADER_SIZE; i++) {
		TEST_ASSERT_EQUAL_HEX8(0x00, buf[i]);
	}
	/* Reserved between SIZE and DATA */
	for (int i = OFFSET_HEADER_SIZE + 2; i < OFFSET_HEADER_DATA; i++) {
		TEST_ASSERT_EQUAL_HEX8(0x00, buf[i]);
	}
}

void test_build_binary_size_field_is_21(void)
{
	build_reference();
	TEST_ASSERT_EQUAL_UINT16(VALID_DATA_BYTES, read_u16_be(buf + OFFSET_HEADER_SIZE));
}

/* ---- CRC fields ---- */

void test_build_binary_dchk_covers_data_area(void)
{
	build_reference();
	uint16_t expected = crc16_ccitt(buf + OFFSET_HEADER_DATA, SIZE_DATA);
	TEST_ASSERT_EQUAL_HEX16(expected, read_u16_be(buf + OFFSET_HEADER_DCHK));
}

void test_build_binary_hchk_covers_header_remainder(void)
{
	build_reference();
	uint16_t expected = crc16_ccitt(buf + OFFSET_HEADER_SIG, SIZE_HEADER - OFFSET_HEADER_SIG);
	TEST_ASSERT_EQUAL_HEX16(expected, read_u16_be(buf + OFFSET_HEADER_HCHK));
}

/* ---- Independence: different inputs produce different output ---- */

void test_build_binary_different_mac_changes_output(void)
{
	uint8_t       buf2[SIZE_TOTAL];
	const uint8_t other_mac[SIZE_MAC] = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0x11};

	build_reference();
	build_binary(other_mac, REF_MAC2, REF_MAJOR, REF_MINOR, REF_SN, buf2);

	TEST_ASSERT_FALSE(memcmp(buf + OFFSET_HEADER_DATA + OFFSET_DATA_MAC1,
				 buf2 + OFFSET_HEADER_DATA + OFFSET_DATA_MAC1, SIZE_MAC) == 0);
	TEST_ASSERT_NOT_EQUAL(read_u16_be(buf + OFFSET_HEADER_DCHK),
			      read_u16_be(buf2 + OFFSET_HEADER_DCHK));
}

void run_build_binary_tests(void)
{
	RUN_TEST(test_build_binary_mac1_placed_correctly);
	RUN_TEST(test_build_binary_mac2_placed_correctly);
	RUN_TEST(test_build_binary_major_stored_big_endian);
	RUN_TEST(test_build_binary_minor_stored_big_endian);
	RUN_TEST(test_build_binary_sn_placed_correctly);
	RUN_TEST(test_build_binary_data_reserved_zeroed);
	RUN_TEST(test_build_binary_signature_correct);
	RUN_TEST(test_build_binary_header_reserved_zeroed);
	RUN_TEST(test_build_binary_size_field_is_21);
	RUN_TEST(test_build_binary_dchk_covers_data_area);
	RUN_TEST(test_build_binary_hchk_covers_header_remainder);
	RUN_TEST(test_build_binary_different_mac_changes_output);
}
