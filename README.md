# TinyQV SDK: Minimal area RISC-V core and accessories

C SDK for the [tinyQV](https://github.com/MichaelBell/tinyQV/) quad serial RISC-V SoC.

## Compiling a project

Get the [customised Risc-V GNU toolchain](https://github.com/MichaelBell/riscv-gnu-toolchain) for TinyQV.

Either download the release from that repo, or clone it and make with:

    ./configure --prefix=/opt/riscv --with-arch=rv32ec --with-abi=ilp32e
    make

You may want to use a different prefix.

Note that this takes a while, especially first time as it downloads a bunch of submodules as part of the build process.

Use the example-project-Makefile as a template Makefile for your project.
