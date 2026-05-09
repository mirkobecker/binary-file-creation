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

#endif /* BINARY_H */
