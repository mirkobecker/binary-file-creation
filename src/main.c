#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "binary.h"

int main(int argc, char *argv[])
{
	if (argc != 6) {
		fprintf(stderr, "Usage: %s <MAC1> <MAC2> <SerialNumber> <Major> <Minor>\n",
			argv[0]);
		return EXIT_FAILURE;
	}

	uint8_t mac1[6];
	switch (parse_mac(argv[1], mac1)) {
	case MAC_OK:
		break;
	case MAC_ERR_LENGTH:
		fprintf(stderr, "Error: MAC1 must be exactly 12 hex characters.\n");
		return EXIT_FAILURE;
	case MAC_ERR_CHAR:
		fprintf(stderr, "Error: MAC1 contains invalid characters (use [0-9A-F]).\n");
		return EXIT_FAILURE;
	case MAC_ERR_BROADCAST:
		fprintf(stderr, "Error: MAC1 broadcast address FFFFFFFFFFFF is not allowed.\n");
		return EXIT_FAILURE;
	default:
		fprintf(stderr, "Error: invalid MAC1.\n");
		return EXIT_FAILURE;
	}

	uint8_t mac2[6];
	switch (parse_mac(argv[2], mac2)) {
	case MAC_OK:
		break;
	case MAC_ERR_LENGTH:
		fprintf(stderr, "Error: MAC2 must be exactly 12 hex characters.\n");
		return EXIT_FAILURE;
	case MAC_ERR_CHAR:
		fprintf(stderr, "Error: MAC2 contains invalid characters (use [0-9A-F]).\n");
		return EXIT_FAILURE;
	case MAC_ERR_BROADCAST:
		fprintf(stderr, "Error: MAC2 broadcast address FFFFFFFFFFFF is not allowed.\n");
		return EXIT_FAILURE;
	default:
		fprintf(stderr, "Error: invalid MAC2.\n");
		return EXIT_FAILURE;
	}

	uint8_t sn[5];
	switch (parse_serial_number(argv[3], sn)) {
	case SN_OK:
		break;
	case SN_ERR_LENGTH:
		fprintf(stderr, "Error: serial number must be exactly 7 characters.\n");
		return EXIT_FAILURE;
	case SN_ERR_YEAR:
		fprintf(stderr, "Error: invalid year character in serial number.\n");
		return EXIT_FAILURE;
	case SN_ERR_MONTH:
		fprintf(stderr, "Error: invalid month character in serial number.\n");
		return EXIT_FAILURE;
	case SN_ERR_TYPE:
		fprintf(stderr, "Error: invalid device type character in serial number.\n");
		return EXIT_FAILURE;
	case SN_ERR_SEQ:
		fprintf(stderr, "Error: serial number sequence must be 4 decimal digits.\n");
		return EXIT_FAILURE;
	default:
		fprintf(stderr, "Error: invalid serial number.\n");
		return EXIT_FAILURE;
	}

	uint16_t major;
	if (parse_version(argv[4], &major) != VER_OK) {
		fprintf(stderr, "Error: major version must be exactly 2 decimal digits [00-99].\n");
		return EXIT_FAILURE;
	}

	uint16_t minor;
	if (parse_version(argv[5], &minor) != VER_OK) {
		fprintf(stderr, "Error: minor version must be exactly 2 decimal digits [00-99].\n");
		return EXIT_FAILURE;
	}

	uint8_t buf[1024];
	build_binary(mac1, mac2, major, minor, sn, buf);

	FILE *f = fopen("output.bin", "wb");
	if (!f) {
		fprintf(stderr, "Error: failed to write output.bin.\n");
		return EXIT_FAILURE;
	}
	if (fwrite(buf, 1, sizeof(buf), f) != sizeof(buf)) {
		fclose(f);
		remove("output.bin");
		fprintf(stderr, "Error: failed to write output.bin.\n");
		return EXIT_FAILURE;
	}
	fclose(f);

	return EXIT_SUCCESS;
}
