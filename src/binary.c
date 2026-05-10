#include "binary.h"
#include <stddef.h>
#include <string.h>

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
		int  val;
	} table[] = {
		{'W',  8}, {'X',  9}, {'A', 10}, {'B', 11}, {'C', 12}, {'D', 13},
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
	sn[3] = (uint8_t)(seq >> 8);
	sn[4] = (uint8_t)(seq & 0xFF);
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
	if (memcmp(tmp, broadcast, 6) == 0) {
		return MAC_ERR_BROADCAST;
	}

	memcpy(mac, tmp, 6);
	return MAC_OK;
}
