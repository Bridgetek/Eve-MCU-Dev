# EVE-MCU-Dev Race Car Dashboard Raspberry Pi pico VSCode Example

[Back](../README.md)

## Compiling using the pico VSCode Extension

The workspace file `racecar.code-workspace` can be loaded directly in Visual Studio Code (VS Code) to load the project as a workspace.

### Setting Up the Race Car Dashboard Raspberry Pi pico VSCode Example

The build environment depends on the presence of the Raspberry Pi pico VSCode Extension. This can be setup following instructions in the [Getting Started With pico](https://datasheets.raspberrypi.com/pico/getting-started-with-pico.pdf) document from the Raspberry Pi website.

### Compiling the Race Car Dashboard Raspberry Pi pico VSCode Example

The instructions for compiling and programming the pico can be followed from the Raspberry Pi pico Extension Guide available from the Raspberry Pi website.

## Compiling using the Command Line

The pico toolchain is also available from the command line. It can be accessed from the command line if the Raspberry Pi pico VSCode Extension is loaded or the Raspberry Pi pico toolchain is configured manually (see "Appendix C: Manual toolchain setup" in the Getting Started With pico document).

The build depends on the `PICO_SDK_PATH` environment variable. This __must__ be set to the correct Pico SDK directory on all platforms before building.

### Setting Up the Race Car Dashboard Raspberry Pi pico Example Manually

To configure the build environment using CMake:

```console
    cmake --fresh  -G "Unix Makefiles" -B build -S .
```

This will configure the compilation to take place in the `build` directory. It must be directed to use `Unix Makefiles` as a generator, if other compilation environments are configured then these may be used in preference to the pico toolchain.

Configuration settings that are normally made in `EVE_config.h` can also be set in the CMake configuration command:

```console
    cmake --fresh  -G "Unix Makefiles" -B build -DEVE_DEVICE=EVE_FT800 -DEVE_DISPLAY_RES=EVE_WQVGA -S .
```

### Compiling the Race Car Dashboard Raspberry Pi pico Example Manually

The following CMake command will build the code and place the resulting files in the `build` directory:

```console
    cmake --build build
```

The UF2 file for programming onto the board is `racecar_pico.uf2` in the `build` directory.

## Running the Example

If the `ASSETS` macro is set to `USE_FLASH` then the appropriate flash image corresponding to the EVE generation must be programmed into the device flash. The options for the image are found in the `eve3`/`eve4`/`eve5` subdirectory. If `USE_C_ARRAYS` then no additional action is needed. The `USE_FILES` or `USE_FLASHIMAGE` options are not possible on the pico platform as there is no file system built into the example.

Note: the default setting for `ASSETS` on pico is `USE_FLASH` which is set in the `CMakeLists.txt` file. 
