RISCV_TOOLCHAIN ?= /opt/tinyQV

CC = $(RISCV_TOOLCHAIN)/bin/riscv32-unknown-elf-gcc
AS = $(RISCV_TOOLCHAIN)/bin/riscv32-unknown-elf-as
AR = $(RISCV_TOOLCHAIN)/bin/riscv32-unknown-elf-ar

all: tinyQV.a tinyQV-sd.a start.o

clean:
	rm -f *.o *.a fatfs/*.o sdcard/*.o

%.o: %.c 
	$(CC) -O2 -march=rv32ec_zcb -mabi=ilp32e -nostdlib -nostartfiles -ffreestanding -ffunction-sections -fdata-sections -Wall -Werror -lc -I$(PWD) -c $< -o $@

%.o: %.s
	$(AS) -march=rv32ec_zicsr_zcb -mabi=ilp32e $< -o $@

tinyQV.a: uart.o uart_buf.o mul.o isqrt.o peripheral.o runtime.o spi.o
	$(AR) rcs $@ $^

tinyQV-sd.a: sdcard/sdcard.o fatfs/ff.o fatfs/ffsystem.o fatfs/ffunicode.o
	$(AR) rcs $@ $^
