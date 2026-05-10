#ifndef BINARY_H
#define BINARY_H

#include <stdint.h>

typedef enum {
	MAC_OK = 0,
	MAC_ERR_NULL,
	MAC_ERR_LENGTH,
	MAC_ERR_CHAR,
	MAC_ERR_BROADCAST,
} mac_err_t;

typedef enum {
	SN_OK = 0,
	SN_ERR_NULL,
	SN_ERR_LENGTH,
	SN_ERR_YEAR,
	SN_ERR_MONTH,
	SN_ERR_TYPE,
	SN_ERR_SEQ,
} sn_err_t;

/**
 * @brief Parse and validate a 7-character serial number string.
 *
 * Format: <Year><Month><Type><NNNN>
 * Year:  W=2008..L=2019, M=2020..X=2029 (W/X ambiguous: first occurrence wins)
 * Month: 1-9=Jan-Sep, A=Oct, B=Nov, C=Dec
 * Type:  4=Typ1, 5=Typ2, 6=Typ3, 7=Typ4, 8=Typ5, 9=Typ6, A=Typ7
 * NNNN:  4 decimal digits 0000-9999
 *
 * @param str  7-character serial number string.
 * @param sn   Output buffer for 5 decoded bytes: [0]=year-2000, [1]=month(1-12),
 *             [2]=type(1-7), [3-4]=seq as big-endian uint16. Unchanged on error.
 * @return     SN_OK on success, or a specific SN_ERR_* code on failure.
 */
sn_err_t parse_serial_number(const char *str, uint8_t sn[5]);

/**
 * @brief Parse and validate a MAC address string.
 *
 * Accepts exactly 12 uppercase hex characters ([0-9A-F]).
 * The broadcast address FFFFFFFFFFFF is rejected.
 *
 * @param str  12-character uppercase hex string without separators.
 * @param mac  Output buffer for 6 decoded bytes. Unchanged on error.
 * @return     MAC_OK on success, or a specific MAC_ERR_* code on failure.
 */
mac_err_t parse_mac(const char *str, uint8_t mac[6]);

/**
 * @brief Assemble the 1024-byte binary blob from validated field values.
 *
 * Layout: 24-byte header followed by a 1000-byte data area.
 * Header fields (big-endian): HCHK at 0x00, signature 'WREC' at 0x02,
 * DCHK at 0x08, valid-byte count (21) at 0x10.  All reserved bytes are 0x00.
 * Data area fields (big-endian): MAC1 at +0x00, MAC2 at +0x06,
 * major at +0x0C, minor at +0x0E, serial number at +0x10.
 *
 * @param mac1   6 decoded MAC address bytes for the first interface.
 * @param mac2   6 decoded MAC address bytes for the second interface.
 * @param major  Major version (uint16, stored big-endian).
 * @param minor  Minor version (uint16, stored big-endian).
 * @param sn     5 decoded serial number bytes.
 * @param buf    Output buffer of exactly 1024 bytes; fully overwritten.
 */
void build_binary(const uint8_t mac1[6], const uint8_t mac2[6], uint16_t major, uint16_t minor,
		  const uint8_t sn[5], uint8_t buf[1024]);

#endif /* BINARY_H */
