all: nanoV.a nanoV-femto.a start.o

clean:
	rm *.o *.a

%.o: %.c 
	riscv32-unknown-elf-gcc -O2 -march=rv32e -mabi=ilp32e -nostdlib -nostartfiles -ffreestanding -ffunction-sections -fdata-sections -lc -c $< -o $@

%.o: %.s
	riscv32-unknown-elf-as -march=rv32em -mabi=ilp32e $< -o $@

nanoV.a: uart.o mul.o peripheral.o runtime.o soft-spi.o isqrt.o
	riscv32-unknown-elf-ar rcs $@ $^ /opt/riscv/lib/gcc/riscv32-unknown-elf/12.2.0/libgcc.a

nanoV-femto.a: uart.o peripheral.o runtime.o soft-spi.o
	riscv32-unknown-elf-ar rcs $@ $^ /opt/riscv/lib/gcc/riscv32-unknown-elf/12.2.0/libgcc.a

