#ifndef CRC_H
#define CRC_H

#include <stddef.h>
#include <stdint.h>

/**
 * @brief Compute CRC-16/CCITT (IBM-3740) over a byte buffer.
 *
 * @param data  Pointer to the input buffer. May be NULL when @p len is 0.
 * @param len   Number of bytes to process.
 * @return      16-bit CRC value. Returns 0xFFFF for an empty input.
 */
uint16_t crc16_ccitt(const uint8_t *data, size_t len);

#endif /* CRC_H */
