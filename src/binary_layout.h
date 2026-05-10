#ifndef BINARY_LAYOUT_H
#define BINARY_LAYOUT_H

/* Field sizes */
#define SIZE_MAC  6u
#define SIZE_VER  2u
#define SIZE_SN   5u
#define SIZE_SIG  4u

#define VALID_DATA_BYTES (SIZE_MAC + SIZE_MAC + SIZE_VER + SIZE_VER + SIZE_SN)

/* Absolute header offsets (bytes from start of buffer) */
#define OFFSET_HEADER_HCHK 0x00u
#define OFFSET_HEADER_SIG  0x02u
#define OFFSET_HEADER_DCHK 0x08u
#define OFFSET_HEADER_SIZE 0x10u
#define OFFSET_HEADER_DATA 0x18u

/* Header and data region sizes */
#define SIZE_HEADER (OFFSET_HEADER_DATA)
#define SIZE_DATA   1000u
#define SIZE_TOTAL  1024u

/* Data area offsets (relative to OFFSET_HEADER_DATA) */
#define OFFSET_DATA_MAC1  0u
#define OFFSET_DATA_MAC2  (OFFSET_DATA_MAC1 + SIZE_MAC)
#define OFFSET_DATA_MAJOR (OFFSET_DATA_MAC2 + SIZE_MAC)
#define OFFSET_DATA_MINOR (OFFSET_DATA_MAJOR + SIZE_VER)
#define OFFSET_DATA_SN    (OFFSET_DATA_MINOR + SIZE_VER)
#define OFFSET_DATA_RES   (OFFSET_DATA_SN + SIZE_SN)

#endif /* BINARY_LAYOUT_H */
