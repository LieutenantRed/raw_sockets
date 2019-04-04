#include "components.h"

uint16_t ip_checksum(void* ip_p) {
    // Cast the data pointer to one that can be indexed.
    char* data = (char*) ip_p;
    size_t length = 20;

    // Initialise the accumulator.
    uint32_t acc = 0;

    // Handle complete 16-bit blocks.
    for (size_t i = 0; i + 1 < length; i += 2) {
        uint16_t word;
        memcpy(&word, data + i, 2);
        acc += htons(word);
        while (acc > 0xffff) {
            acc -= 0xffff;
        }
    }

    // Return the checksum in network byte order.
    return htons(~acc);
}
/*
  Name  : CRC-32
  Poly  : 0x04C11DB7    x^32 + x^26 + x^23 + x^22 + x^16 + x^12 + x^11
                       + x^10 + x^8 + x^7 + x^5 + x^4 + x^2 + x + 1
  Init  : 0xFFFFFFFF
  Revert: true
  XorOut: 0xFFFFFFFF
  Check : 0xCBF43926 ("123456789")
  MaxLen: 268 435 455 byte (2 147 483 647 бит)
*/
uint32_t crc32(unsigned char *buf, size_t len)
{
    uint32_t crc_table[256];
    uint32_t crc; int i, j;

    for (i = 0; i < 256; i++)
    {
        crc = i;
        for (j = 0; j < 8; j++)
            crc = crc & 1 ? (crc >> 1) ^ 0xEDB88320UL : crc >> 1;

        crc_table[i] = crc;
    };

    crc = 0xFFFFFFFFUL;

    while (len--)
        crc = crc_table[(crc ^ *buf++) & 0xFF] ^ (crc >> 8);

    return crc ^ 0xFFFFFFFFUL;
}