# TinyQV SDK: Minimal area RISC-V core and accessories

C SDK for the [tinyQV](https://github.com/MichaelBell/tinyQV/) quad serial RISC-V SoC.

## Compiling a project

Get the [customised Risc-V GNU toolchain](https://github.com/MichaelBell/riscv-gnu-toolchain) for TinyQV.

Either download the release from that repo, or clone it and make with:

    ./configure --prefix=/opt/tinyQV --with-arch=rv32ec_zcb_zicond --with-abi=ilp32e
    make

Note that this takes a while, especially first time as it downloads a bunch of submodules as part of the build process.

If the toolchain is not installed in /opt/tinyQV, then set the `RISCV_TOOLCHAIN` environment variable appropriately.

Use the `example-project` as a template for your project if building for full TinyQV, or `example-sim-project` if building for simulation (with limited RAM and flash).

## Choosing the correct version

The main branch is suitable for the ttsky25a, ttihp25b and ttgf0p2 versions of TinyQV.  Use the tt06 branch for tt06.

For TinyQV on Wafer Space run 1, use ws01 branch.
