#pragma once

#include <stdint.h>
#include <stdbool.h>

// Low level SPI send function.  Data is:
//   Bits 0-7: Byte to send
//   Bit 8: High to end txn, if low CS remains asserted after byte is sent
//   Bit 9: DC control
// The send is scheduled and proceeds in the background, the received byte is ignored
void spi_send_data(uint32_t data);

// Low level SPI send and receive function.  Data is:
//   Bits 0-7: Byte to send
//   Bit 8: High to end txn, if low CS remains asserted after byte is sent
//   Bit 9: DC control
// This function blocks and returns the received byte
uint8_t spi_recv_byte(uint32_t data);

// Set the SPI clock divider to 2 * (divider + 1).
// The valid range of the argument is 0-3, giving clock dividers of 2 - 8.
// At reset the default clock divider is 4.
void spi_set_divider(uint32_t divider);

// Send multiple bytes, ending the transaction if specified
void spi_send_bytes(const uint8_t* data, int len, bool end_txn, bool set_dc);
