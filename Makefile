RISCV_TOOLCHAIN ?= /opt/femtorv

CC = $(RISCV_TOOLCHAIN)/bin/riscv32-unknown-elf-gcc
AS = $(RISCV_TOOLCHAIN)/bin/riscv32-unknown-elf-as
AR = $(RISCV_TOOLCHAIN)/bin/riscv32-unknown-elf-ar

all: femtorv.a femtorv-sd.a start.o

clean:
	rm -f *.o *.a fatfs/*.o sdcard/*.o

%.o: %.c 
	$(CC) -O2 -march=rv32i -nostdlib -nostartfiles -ffreestanding -ffunction-sections -fdata-sections -Wall -Werror -lc -I$(PWD) -c $< -o $@

%.o: %.s
	$(AS) -march=rv32i $< -o $@

femtorv.a: uart.o isqrt.o peripheral.o runtime.o spi.o
	$(AR) rcs $@ $^

femtorv-sd.a: sdcard/sdcard.o fatfs/ff.o fatfs/ffsystem.o fatfs/ffunicode.o
	$(AR) rcs $@ $^
