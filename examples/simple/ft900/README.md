# EVE-MCU-Dev Simple FT9xx Example

[Back](../README.md)

## Setting Up the FT9xx Toolchain

The build environment depends on the presence of the FT9xx Toolchain. This can be downloaded and setup following instructions in the [FT9xx Toolchain](https://brtchip.com/ft9xx-toolchain/) page on the BridgeTek website. The document *AN 325 FT9xx Toolchain Installation and Start Guide* on that page provides a step-by-step guide.

The toolchain will configure a command line environment for compilation of FT9xx projects. It is intended for Windows environments.

A Visual Studio Code extension for the FT9xx devices is also available from the VS Code Extensions Activity Bar. The extension name is "Bridgetek FT9xx Extension" from "Bridgetek". This will automate and manage the build environment.

## Compiling the FT9xx Example

The project can be compiled on the command line or in Visual Studio Code using the Bridgetek FT9xx Extension. The toolchain install will make an evironment variable called `FT9XX_TOOLCHAIN` pointing to the FT9xx Toolchain installation that is used in the build process.

### Compiling the FT9xx Example on the Command Line

The build process is controlled by a file in this folder called `CMakeLists.txt`. This is used by utilities called "CMake" and "Ninja" to perform the build.

Firstly, the CMake environment must be configured first with the command. This line will make a debug build for an FT90X target. The `Debug` parameter can be changed to `Release` to remove debug information from the buils. The `ft90x` target can be changed to `ft93x` for an FT93X target device.

```
cmake -G "Ninja" -S . -B build -DBUILD=Debug -DTARGET=ft90x
```

Configuration files for the build are now kept in the directory called `build` after that command completes.

Then the compilation performed using this command.

```
cmake --build build
```

Once completed, the compiled binary files are found in the `FT90X_Debug` directory for the standard build above. For FT90X release builds the directory is `FT90X_Release`, for FT93X debug builds it is `FT93X_Debug` and finally FT93X release builds it is `FT93X_Release`.

### Programming the FT9xx Example from the Command Line

The executable image is called `simple_ft9xx.bin`. This file must be programmed into the FT9XX device to run. The utility `FT9xxProg.exe` is called to program either the Flash or the Program Memory. Please refer to the *AN 325 FT9xx Toolchain Installation and Start Guide* for further details.

To program the image into the Flash use the following command line:
```
FT9xxProg.exe --loadflash "FT90x_Debug\simple_ft9xx.bin" --onewire -D 0
```

### Compiling the FT9xx Example on VSCode

The build, programming and debug process is described in the [FT9xx Toolchain](https://brtchip.com/ft9xx-toolchain/) page on the BridgeTek website.
