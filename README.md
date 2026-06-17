# FemtoRV SDK: Minimal RISC-V SoC

C SDK for the [femtoRV](https://github.com/MichaelBell/ttsky25b-femtorv-soc/) RISC-V SoC.

## Compiling a project

You will need an appropriate RV32I RISC-V toolchain.  You can get this by cloning https://github.com/riscv-collab/riscv-gnu-toolchain and then make with:

    ./configure --prefix=/opt/femtorv --with-arch=rv32i
    make

Note that this takes a while, especially first time as it downloads a bunch of submodules as part of the build process.

If the toolchain is not installed in /opt/femtorv, then set the `RISCV_TOOLCHAIN` environment variable appropriately.

Use the `example-project` as a template for your project.
