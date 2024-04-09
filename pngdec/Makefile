RISCV_TOOLCHAIN ?= /opt/tinyQV

CC = $(RISCV_TOOLCHAIN)/bin/riscv32-unknown-elf-gcc
AS = $(RISCV_TOOLCHAIN)/bin/riscv32-unknown-elf-as
AR = $(RISCV_TOOLCHAIN)/bin/riscv32-unknown-elf-ar

all: tinyQV-pngdec.a

clean:
	rm -f *.o *.a fatfs/*.o sdcard/*.o

%.o: %.c 
	$(CC) -O2 -march=rv32ec_zcb -mabi=ilp32e -nostdlib -nostartfiles -ffreestanding -ffunction-sections -fdata-sections -Wall -Werror -lc -I$(PWD) -c $< -o $@

%.o: %.s
	$(AS) -march=rv32ec_zicsr_zcb -mabi=ilp32e $< -o $@

tinyQV-pngdec.a: PNGdec.o adler32.o crc32.o infback.o inffast.o inflate.o inftrees.o zutil.o
	$(AR) rcs $@ $^
