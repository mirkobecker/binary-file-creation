#include "binary.h"
#include <string.h>

static int hex_nibble(char c)
{
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    return -1;
}

mac_err_t parse_mac(const char *str, uint8_t mac[6])
{
    if (str == NULL)
        return MAC_ERR_NULL;
    if (strlen(str) != 12)
        return MAC_ERR_LENGTH;

    uint8_t tmp[6];
    for (int i = 0; i < 6; i++) {
        int hi = hex_nibble(str[i * 2]);
        int lo = hex_nibble(str[i * 2 + 1]);
        if (hi < 0 || lo < 0)
            return MAC_ERR_CHAR;
        tmp[i] = (uint8_t)((hi << 4) | lo);
    }

    static const uint8_t broadcast[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    if (memcmp(tmp, broadcast, 6) == 0)
        return MAC_ERR_BROADCAST;

    memcpy(mac, tmp, 6);
    return MAC_OK;
}
