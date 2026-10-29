# EVE-MCU-Dev Submarine Raspberry Pi pico VSCode Example

[Back](../README.md)

## Compiling using the pico VSCode Extension

The workspace file `submarine.code-workspace` can be loaded directly in Visual Studio Code (VS Code) to load the project as a workspace.

### Setting Up the Submarine Raspberry Pi pico VSCode Example

The build environment depends on the presence of the Raspberry Pi pico VSCode Extension. This can be setup following instructions in the [Getting Started With pico](https://datasheets.raspberrypi.com/pico/getting-started-with-pico.pdf) document from the Raspberry Pi website.

### Compiling the Submarine Raspberry Pi pico VSCode Example

The instructions for compiling and programming the pico can be followed from the Raspberry Pi pico Extension Guide available from the Raspberry Pi website.

## Compiling using the Command Line

The pico toolchain is also available from the command line. It can be accessed from the command line if the Raspberry Pi pico VSCode Extension is loaded or the Raspberry Pi pico toolchain is configured manually (see "Appendix C: Manual toolchain setup" in the Getting Started With pico document).

### Setting Up the Submarine Raspberry Pi pico Example Manually

To configure the build environment using CMake:

```console
    cmake --fresh  -G "Unix Makefiles" -B build -S .
```

This will configure the compilation to take place in the `build` directory. It must be directed to use `Unix Makefiles` as a generator, if other compilation environments are configured then these may be used in preference to the pico toolchain.

Configuration settings that are normally made in `EVE_config.h` can also be set in the CMake configuration command:

```console
    cmake --fresh  -G "Unix Makefiles" -B build -DEVE_DEVICE=EVE_FT800 -DEVE_DISPLAY_RES=EVE_WQVGA -S .
```

### Compiling the Submarine Raspberry Pi pico Example Manually

The following CMake command will build the code and place the resulting files in the `build` directory:

```console
    cmake --build build
```

The UF2 file for programming onto the board is `submarine_pico.uf2` in the `build` directory.

## Persistent Storage

In the [main.c](main/main.c) program there are the following functions used to store calibration data in flash `platform_calib_init()`, `platform_calib_read()`, `platform_calib_write()`.

In this example the calibration data is stored by these functions in flash program memory at the very top sector of flash.

This method is not recommended usage as a program may utilise this flash area and the calibration data may overwrite the program.
The implementation here is simply to show how the calibration information is obtained and stored.
Other methods should be explored for a completed application.

A proper implementation of this method in an application would modify the linker script to reserve this area for the storage of any persistent data that is required for an application.
Other methods of storing calibration data may be utilised such as writing to the flash attached to the EVE or external devices connected to the pico.

