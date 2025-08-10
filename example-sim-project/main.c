#include <stddef.h>
#include <timer.h>
#include <gpio.h>
#include <uart.h>

// Get the base address for accessing the peripheral
volatile uint8_t* ledstrip = (volatile uint8_t*)PERI_BASE_ADDRESS(18);

// Define names for the registers
#define REG_CTRL 0
#define REG_G 2
#define REG_R 1
#define REG_B 3
#define REG_CHAR 4

uint8_t next_brightness = 1;

// Timer that does something
void timer_callback(void*) {
    // Repeat every 1ms
    set_alarm(1, timer_callback, NULL);

    // Set up colour
    ledstrip[REG_R] = next_brightness;
    ledstrip[REG_G] = 0;
    ledstrip[REG_B] = next_brightness;

    // Write one LED
    ledstrip[REG_CTRL] = 1;

    // Wait for LED
    while (ledstrip[REG_CTRL] == 0);

    // Set next colour
    ledstrip[REG_R] = 0;
    ledstrip[REG_G] = next_brightness;
    ledstrip[REG_B] = 0;

    // Write one LED and reset
    ledstrip[REG_CTRL] = 0x81;

    ++next_brightness;
}

int main(void) {
    // Check UART is working.
    // To keep the binary small use uart_putc, uart_puts.
    // If you need printf, then use uart_printf as it is a bit smaller than the regular implementation
    uart_puts("Hello, world!");

    // Enable all outputs (ensure they are not in debug mode)
    enable_all_outputs();

    // Set out1 to peripheral 18
    set_gpio_func(1, 18);

    // Manually call the update timer callback, this will
    // then set the timer to call it back at regular intervals
    timer_callback(NULL);

    // Loop forever - all further work will be done in the
    // timer interrupt.
    while(1);
}
