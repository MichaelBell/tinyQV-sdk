RISCV_TOOLCHAIN ?= /opt/riscv

CC = $(RISCV_TOOLCHAIN)/bin/riscv32-unknown-elf-gcc
AS = $(RISCV_TOOLCHAIN)/bin/riscv32-unknown-elf-as
AR = $(RISCV_TOOLCHAIN)/bin/riscv32-unknown-elf-ar

all: tinyQV.a start.o

clean:
	rm *.o *.a

%.o: %.c 
	$(CC) -O2 -march=rv32ec -mabi=ilp32e -nostdlib -nostartfiles -ffreestanding -ffunction-sections -fdata-sections -lc -c $< -o $@

%.o: %.s
	$(AS) -march=rv32emc_zicsr -mabi=ilp32e $< -o $@

tinyQV.a: uart.o uart_buf.o mul.o isqrt.o peripheral.o runtime.o
	$(AR) rcs $@ $^ $(RISCV_TOOLCHAIN)/lib/gcc/riscv32-unknown-elf/*/libgcc.a
