#include "binary.h"
#include "binary_layout.h"
#include "crc.h"
#include <stddef.h>
#include <string.h>

#define SIG_BYTES "WREC"

static void write_u16_be(uint8_t *buf, uint16_t val)
{
	buf[0] = (uint8_t)(val >> 8);
	buf[1] = (uint8_t)(val & 0xFF);
}

static int hex_nibble(char c)
{
	if (c >= '0' && c <= '9') {
		return c - '0';
	}
	if (c >= 'A' && c <= 'F') {
		return c - 'A' + 10;
	}
	return -1;
}

/* Maps year character to (year - 2000). First occurrence of W/X wins (2008/2009). */
static int parse_year(char c)
{
	static const struct {
		char ch;
		int val;
	} table[] = {
		{'W', 8},  {'X', 9},  {'A', 10}, {'B', 11}, {'C', 12}, {'D', 13},
		{'E', 14}, {'F', 15}, {'H', 16}, {'J', 17}, {'K', 18}, {'L', 19},
		{'M', 20}, {'N', 21}, {'P', 22}, {'R', 23}, {'S', 24}, {'T', 25},
		{'U', 26}, {'V', 27}, {'W', 28}, {'X', 29},
	};
	for (size_t i = 0; i < sizeof(table) / sizeof(table[0]); i++) {
		if (table[i].ch == c) {
			return table[i].val;
		}
	}
	return -1;
}

/* Maps month character to month number (1-12). */
static int parse_month(char c)
{
	if (c >= '1' && c <= '9') {
		return c - '0';
	}
	if (c == 'A') {
		return 10;
	}
	if (c == 'B') {
		return 11;
	}
	if (c == 'C') {
		return 12;
	}
	return -1;
}

/* Maps device type character to type number (1-7). */
static int parse_type(char c)
{
	if (c >= '4' && c <= '9') {
		return c - '4' + 1;
	}
	if (c == 'A') {
		return 7;
	}
	return -1;
}

ver_err_t parse_version(const char *str, uint16_t *out)
{
	if (str == NULL) {
		return VER_ERR_NULL;
	}
	if (strlen(str) != 2) {
		return VER_ERR_LENGTH;
	}
	if (str[0] < '0' || str[0] > '9' || str[1] < '0' || str[1] > '9') {
		return VER_ERR_CHAR;
	}
	*out = (uint16_t)((str[0] - '0') * 10 + (str[1] - '0'));
	return VER_OK;
}

sn_err_t parse_serial_number(const char *str, uint8_t sn[5])
{
	if (str == NULL) {
		return SN_ERR_NULL;
	}
	if (strlen(str) != 7) {
		return SN_ERR_LENGTH;
	}

	int year = parse_year(str[0]);
	if (year < 0) {
		return SN_ERR_YEAR;
	}

	int month = parse_month(str[1]);
	if (month < 0) {
		return SN_ERR_MONTH;
	}

	int type = parse_type(str[2]);
	if (type < 0) {
		return SN_ERR_TYPE;
	}

	uint16_t seq = 0;
	for (int i = 3; i < 7; i++) {
		if (str[i] < '0' || str[i] > '9') {
			return SN_ERR_SEQ;
		}
		seq = (uint16_t)(seq * 10 + (str[i] - '0'));
	}

	sn[0] = (uint8_t)year;
	sn[1] = (uint8_t)month;
	sn[2] = (uint8_t)type;
	write_u16_be(sn + 3, seq);
	return SN_OK;
}

mac_err_t parse_mac(const char *str, uint8_t mac[6])
{
	if (str == NULL) {
		return MAC_ERR_NULL;
	}
	if (strlen(str) != 12) {
		return MAC_ERR_LENGTH;
	}

	uint8_t tmp[6];
	for (int i = 0; i < 6; i++) {
		int hi = hex_nibble(str[i * 2]);
		int lo = hex_nibble(str[i * 2 + 1]);
		if (hi < 0 || lo < 0) {
			return MAC_ERR_CHAR;
		}
		tmp[i] = (uint8_t)((hi << 4) | lo);
	}

	static const uint8_t broadcast[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
	if (memcmp(tmp, broadcast, SIZE_MAC) == 0) {
		return MAC_ERR_BROADCAST;
	}

	memcpy(mac, tmp, 6);
	return MAC_OK;
}

void build_binary(const uint8_t mac1[6], const uint8_t mac2[6], uint16_t major, uint16_t minor,
		  const uint8_t sn[5], uint8_t buf[1024])
{
	memset(buf, 0, SIZE_HEADER + SIZE_DATA);

	/* Data area is filled first: DCHK and HCHK depend on its final content. */
	uint8_t *data = buf + OFFSET_HEADER_DATA;

	memcpy(data + OFFSET_DATA_MAC1, mac1, SIZE_MAC);
	memcpy(data + OFFSET_DATA_MAC2, mac2, SIZE_MAC);
	write_u16_be(data + OFFSET_DATA_MAJOR, major);
	write_u16_be(data + OFFSET_DATA_MINOR, minor);
	memcpy(data + OFFSET_DATA_SN, sn, SIZE_SN);

	uint16_t dchk = crc16_ccitt(data, SIZE_DATA);

	memcpy(buf + OFFSET_HEADER_SIG, SIG_BYTES, SIZE_SIG);
	write_u16_be(buf + OFFSET_HEADER_DCHK, dchk);
	write_u16_be(buf + OFFSET_HEADER_SIZE, VALID_DATA_BYTES);

	uint16_t hchk = crc16_ccitt(buf + OFFSET_HEADER_SIG, SIZE_HEADER - OFFSET_HEADER_SIG);
	write_u16_be(buf + OFFSET_HEADER_HCHK, hchk);
}
