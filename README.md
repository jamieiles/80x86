s80x86
======

s80x86 is an 80186 binary compatible CPU, written in SystemVerilog suitable for
synthesis on FPGA or ASIC.

System Requirements
-------------------

- Docker
- 4GB RAM

Building
--------

    ./scripts/build

This will build the Docker containers and then perform a release build and run
all of the tests.

Documentation
-------------

The build script above will create a developer's guide in
`./_build/build/documentation/development-guide.pdf`
