#pragma once

#include <stdint.h>

void soft_spi_setup(int sck, int cs, int mosi, int miso);
void soft_spi_send_bytes(const uint8_t* data, int len);
void soft_spi_recv_bytes(uint8_t* data, int len);
void soft_spi_send_recv_bytes(const uint8_t* send_data, uint8_t* recv_data, int len);