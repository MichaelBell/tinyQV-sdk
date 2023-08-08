# NanoV SDK: Minimal area RISC-V core and accessories

C SDK for the [nanoV](https://github.com/MichaelBell/nanoV/) bit serial RISC-V core and integrated SPI memory controller.

## Compiling a project

Build the [Risc-V GNU toolchain](https://github.com/riscv/riscv-gnu-toolchain) for RV32E using

    ./configure --prefix=/opt/riscv --with-arch=rv32e --with-abi=ilp32e
    make

Use the example-project-Makefile as a template Makefile for your project.
