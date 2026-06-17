PROJECT_NAME ?= example

PROJECT_SOURCES ?= hello.c

RISCV_TOOLCHAIN ?= /opt/femtorv

CC = $(RISCV_TOOLCHAIN)/bin/riscv32-unknown-elf-gcc
AS = $(RISCV_TOOLCHAIN)/bin/riscv32-unknown-elf-as
AR = $(RISCV_TOOLCHAIN)/bin/riscv32-unknown-elf-ar
LD = $(RISCV_TOOLCHAIN)/bin/riscv32-unknown-elf-ld
OBJCOPY = $(RISCV_TOOLCHAIN)/bin/riscv32-unknown-elf-objcopy

SDK ?= ../femtorv-sdk

all: $(PROJECT_NAME).bin $(PROJECT_NAME).hex

clean:
	cd $(dir $(PROJECT_NAME)) && rm *.o *.elf *.bin *.hex

%.o: %.c
	$(CC) -O2 -I$(SDK) -march=rv32i -mno-strict-align -nostdlib -nostartfiles -ffreestanding -ffunction-sections -fdata-sections -Wall -Werror -lc -lm -c $< -o $@

%.o: %.s
	$(AS) -march=rv32i $< -o $@

$(PROJECT_NAME).elf: $(PROJECT_SOURCES:.c=.o)
	$(LD) $^ $(SDK)/start.o $(SDK)/femtorv.a $(RISCV_TOOLCHAIN)/riscv32-unknown-elf/lib/libc.a $(RISCV_TOOLCHAIN)/riscv32-unknown-elf/lib/libm.a $(RISCV_TOOLCHAIN)/lib/gcc/riscv32-unknown-elf/*/libgcc.a -T $(SDK)/memmap_sim --gc-sections -o $@

$(PROJECT_NAME).bin: $(PROJECT_NAME).elf
	$(OBJCOPY) $< -O binary $@

$(PROJECT_NAME).hex: $(PROJECT_NAME).bin
	od -An -t x1 -w4 -v $< > $@
