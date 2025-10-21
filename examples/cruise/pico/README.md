# EVE-MCU-Dev Cruise Raspberry Pi pico VSCode Example

[Back](../README.md)

## Compiling using the pico VSCode Extension

The workspace file `cruise.code-workspace` can be loaded directly in Visual Studio Code (VS Code) to load the project as a workspace.

### Setting Up the Cruise Raspberry Pi pico VSCode Example

The build environment depends on the presence of the Raspberry Pi pico VSCode Extension. This can be setup following instructions in the [Getting Started With pico](https://datasheets.raspberrypi.com/pico/getting-started-with-pico.pdf) document from the Raspberry Pi website.

### Compiling the Cruise Raspberry Pi pico VSCode Example

The instructions for compiling and programming the pico can be followed from the Raspberry Pi pico Extension Guide available from the Raspberry Pi website.

## Compiling using the Command Line

The pico toolchain is also available from the command line. It can be accessed from the command line if the Raspberry Pi pico VSCode Extension is loaded or the Raspberry Pi pico toolchain is configured manually (see "Appendix C: Manual toolchain setup" in the Getting Started With pico document).

### Setting Up the Cruise Raspberry Pi pico Example Manually

To configure the build environment using CMake:

```
cmake --fresh -B build -S . -G "Unix Makefiles"
```

This will configure the compilation to take place in the `build` directory. It must be directed to use `Unix Makefiles` as a generator, if other compilation environments are configured then these may be used in preference to the pico toolchain.

### Compiling the Cruise Raspberry Pi pico Example Manually

The following CMake command will build the code and place the resulting files in the `build` directory:

```
cmake --build build
```

The UF2 file for programming onto the board is `cruise_pico.uf2` in the `build` directory.
