# TinyQV SDK: Minimal area RISC-V core and accessories

C SDK for the [tinyQV](https://github.com/MichaelBell/tinyQV/) quad serial RISC-V SoC.

## Compiling a project

Build the [Risc-V GNU toolchain](https://github.com/riscv/riscv-gnu-toolchain) for RV32E using

    ./configure --prefix=/opt/riscv --with-arch=rv32ec --with-abi=ilp32e
    make

Use the example-project-Makefile as a template Makefile for your project.
