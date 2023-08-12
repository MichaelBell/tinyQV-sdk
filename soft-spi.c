#include "gpio.h"
#include "soft-spi.h"

static int sck, cs, mosi, miso;

void soft_spi_setup(int sck_, int cs_, int mosi_, int miso_)
{
    sck = sck_ >= 0 ? 1 << sck_ : 0;
    cs = cs_ >= 0 ? 1 << cs_ : 0;
    mosi = mosi_ >= 0 ? 1 << mosi_ : 0;
    miso = miso_ >= 0 ? 1 << miso_ : 0;

    int outputs = get_outputs();
    outputs &= ~(sck | cs | mosi);
    outputs |= cs;
    set_outputs(outputs);
}

void soft_spi_send_bytes(const uint8_t* data, int len)
{
    int outputs = get_outputs();
    outputs &= ~(sck | cs | mosi);
    set_outputs(outputs);

    int sck_low_data_low = outputs;
    int sck_low_data_high = outputs | mosi;
    int sck_high_data_low = outputs | sck;
    int sck_high_data_high = outputs | sck | mosi;

#define SEND_BIT(mask) \
        if (d & mask) { \
            set_outputs(sck_low_data_high); \
            set_outputs(sck_high_data_high); \
        } else { \
            set_outputs(sck_low_data_low); \
            set_outputs(sck_high_data_low); \
        }

    for (int i = 0; i < len; ++i)
    {
        uint32_t d = data[i];
        SEND_BIT(0x80);
        SEND_BIT(0x40);
        SEND_BIT(0x20);
        SEND_BIT(0x10);
        SEND_BIT(0x08);
        SEND_BIT(0x04);
        SEND_BIT(0x02);
        SEND_BIT(0x01);
    }

    outputs |= cs;
    set_outputs(outputs);
}