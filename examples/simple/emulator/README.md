# EVE-MCU-Dev Simple Example for the EVE Emulator

Please follow instructions to install the EVE Emulator in EVE-MCU-Dev library. Instructions are available in [EVE-MCU-Dev Ports for the EVE Emulator](../../../ports/eve_emulator/README.md). This must be done before compilation.

This example is not compatible with the EVE API 5 (BT820) as this device is not supported in the EVE Emulator.

## Command Line Compilation

To configure the project with the default C compiler:
```
    cmake -B build -S .
```
On Windows, if an MSVC compiler is not the default compiler on your platform then choose a suitable Visual Studio compiler explicitly:
```
    cmake -B build -S . -G "Visual Studio 17 2022"
```
On Windows, to compile the project use the MinGW C compiler:
```
    cmake -B build -S . -G "MinGW Makefiles"
```
Then to compile:
```
    cmake --build build
```
Default builds are compiled with debugging enabled. 

The executable file is normally `build\Debug\simple_emulator.exe` or  `build\simple_emulator.exe`. This can be run from the command line. 

Note: Necessary DLLs are copied into the `build\Debug` or `build` directory to enable the executable to run.

Configuration settings that are normally made in `EVE_config.h` can also be set in the CMake configuration command:

```
cmake --fresh  -G "MinGW Makefiles" -B build -DFT8XX_TYPE=FT810 -DDISPLAY_RES=WVGA -S .
```

## Visual Studio Code

To use this example with Microsoft Visual Studio Code follow the instructions in this web page.

https://code.visualstudio.com/docs/cpp/config-msvc

It is possible to use the command line instructions above in a terminal windows from within Visual Studio Code.

## Visual Studio

The Visual Studio project files for the example are found in the `VisualStudio` folder. Program executables and object files are placed into the `VisualStudio\Debug` folder. 

## Linux

Linux builds are not supported as the emulator is not distributed for Linux.
