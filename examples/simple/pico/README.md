# EVE-MCU-Dev Simple Raspberry Pi pico Example

[Back](../README.md)

## Setting Up the pico SDK

The build environment depends on the presence of the Raspberry Pi pico SDK. This can be setup following instructions in the section "Appendix C: Manual toolchain setup" of the [Getting Started With pico](https://datasheets.raspberrypi.com/pico/getting-started-with-pico.pdf) document from the Raspberry Pi website.

This will configure a command line environment for compilation of Raspberry Pi pico projects. It is intended for Linux environments.

## Compiling the Simple Raspberry Pi pico Example

The build process is controlled by a file in this folder called `CMakeLists.txt`. This is used by a utility called CMake to perform the build.

Firstly, the CMake environment must be configured first with the command.
```
cmake -B build -S .
```
Configuration files for the build are kept in the directory called `build`.

Then the compilation performed using this command.
```
cmake --build build
```
Once completed, the compiled files are also found in the `build` directory.

## Running the Simple Raspberry Pi pico Example

The executable image is found in the file `build` directory. It is called `simple-pico.uf2`.

To run this file place the pico into BOOTSEL mode. Then copy the file to the USB Mass Storage Device that is started on the pico when BOOTSEL mode is started.
