# s80x86

The S80186 IP core is a compact, 80186 binary compatible core, implementing
the full 80186 ISA suitable for integration into FPGA/ASIC designs.  The core
executes most instructions in far fewer cycles than the original Intel 8086,
and in many cases, fewer cycles than the 80286.  The core is supplied as
synthesizable SystemVerilog, along with a C++ reference model, extensive
tests, a reference BIOS implementation and reference FPGA designs.

## Features

* Binary compatible with the Intel 80186 core
  - MS-DOS
  - FreeDOS
  - OpenWatcom/GCC ia16-elf compilers
* Separate data/IO and instruction busses
* 1MB addressable segmented memory
* 64KB addressable IO space
* Lock signal for atomic accesses
* 256 external interrupts
* 1 non-maskable interrupt
* 3 stage fetch/decode/execute pipeline
* Debug interface suitable
  - JTAG TAP compatible
  - Register/Memory access
  - Execution control
* FPGA builds (Intel Cyclone V)
  - 60MHz FMax
  - ~1800 ALMs

## Implementation

* Full synthesizable SystemVerilog
* C++ reference model
* FPGA reference designs for Terasic DE0-Nano and DE0-CV
* Extensive test suite verified against C++ model, RTL and FPGA
* Complete Docker build environments
* Full development documentation and programmers guide

## Reference Design

* SDR SDRAM controller
* SD card for fixed disk emulation
* 8259 PIC-lite controller
* UART
* 8254 PIT-lite timer operating in mode 2 only.
* CGA controller with VGA output
* PS/2 controller (mouse and keyboard)
* Direct mapped write-back cache

## System Requirements

* Docker
* 4GB RAM

## Building

    ./scripts/build

This will build the Docker containers and then perform a release build and run
all of the tests.

## Documentation

The build script above will create a developer's guide in
`./_build/build/documentation/development-guide.pdf`
