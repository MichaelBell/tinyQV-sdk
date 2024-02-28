all: tinyQV.a start.o

clean:
	rm *.o *.a

%.o: %.c 
	riscv32-unknown-elf-gcc -O2 -march=rv32ec -mabi=ilp32e -nostdlib -nostartfiles -ffreestanding -ffunction-sections -fdata-sections -lc -c $< -o $@

%.o: %.s
	riscv32-unknown-elf-as -march=rv32emc_zicsr -mabi=ilp32e $< -o $@

tinyQV.a: uart.o uart_buf.o mul.o isqrt.o peripheral.o runtime.o
	riscv32-unknown-elf-ar rcs $@ $^ /opt/riscv/lib/gcc/riscv32-unknown-elf/12.2.0/libgcc.a
