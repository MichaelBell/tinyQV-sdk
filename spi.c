#include "spi.h"

void spi_send_bytes(const uint8_t* data, int len, bool end_txn, bool set_dc)
{
    uint32_t mask = set_dc ? 0x200 : 0;
    for (int i = 0; i < len; ++i) {
        if (end_txn && i == len - 1) mask |= 0x100;
        spi_send_data(data[i] | mask);
    }
}
