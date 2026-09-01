# EVE-MCU-Dev MedInfo Example for libft4222

Please follow instructions to install the LibFT4222 Middleware Library in EVE-MCU-Dev library. Instructions are available in [EVE-MCU-Dev Ports for libft4222](../../../ports/eve_libft4222/README.md). This must be done before compilation.

## Command Line Compilation

To configure the project with the default C compiler:
```console
    cmake -B build -S .
```
On Windows, if an MSVC compiler is not the default compiler on your platform then choose a suitable Visual Studio compiler explicitly:
```console
    cmake -B build -S . -G "Visual Studio 17 2022"
```
On Windows, to compile the project use the MinGW C compiler:
```console
    cmake -B build -S . -G "MinGW Makefiles"
```
Then to compile:
```console
    cmake --build build
```
Default builds are compiled with debugging enabled. 

The executable file is `build\Debug\medinfo_libft4222.exe`. This can be run from the command line. 

Note: Necessary DLLs are copied into the `build\Debug` directory to enable the executable to run.

Configuration settings that are normally made in `EVE_config.h` can also be set in the CMake configuration command:

```console
    cmake --fresh  -G "MinGW Makefiles" -B build -DEVE_DEVICE=EVE_FT810 -DEVE_DISPLAY_RES=EVE_WVGA -S .
```

## Visual Studio Code

To use this example with Microsoft Visual Studio Code follow the instructions in this web page.

https://code.visualstudio.com/docs/cpp/config-msvc

It is possible to use the command line instructions above in a terminal windows from within Visual Studio Code.

## Visual Studio

The Visual Studio project files for the example are found in the `VisualStudio` folder. Object files are placed into the `VisualStudio\obj` folder. Program executables are placed into the `VisualStudio\bin` folder. 

The example application will automatically copy either the `LibFT4222-64.dll` or `LibFT4222.dll` DLLs to the executable output directory. 

## Linux

The cmake command line examples for Windows will compile the required files for the example application under Linux. The LibFT4222 library for Linux will be taken from the `/usr/local/lib` directory and the include files from `/usr/local/include`.

## Running the Example

When the `ASSETS` macro is set to `USE_FILES` or `USE_FLASHIMAGE` then the path to the `assets` directory and the `source` directory is required as the first parameter to the program executable. If the macro is set to `USE_FLASH` then the appropriate flash image corresponding to the EVE generation must be programmed into the device flash. The flash image file is `flash-820-default.bin` in the `assets` directory. If `USE_C_ARRAYS` then no additional action is needed.

Note: the default setting for `ASSETS` on libft4222 is `USE_FILES` which is set in the `CMakeLists.txt` file. 

For example:

To use the `USE_FILES` method:
```
> .\build\Debug\medinfo_libft4222.exe ..\assets\source\
```

To use the `USE_FLASHIMAGE` method on BT817 (EVE3):
```
> .\build\Debug\medinfo_libft4222.exe ..\assets\eve3\
```