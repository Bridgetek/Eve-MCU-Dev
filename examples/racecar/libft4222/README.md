# EVE-MCU-Dev Race Car Dashboard Example for libft4222

## Command Line Compilation

To compile the project with the default C compiler:
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

The executable file is `build\Debug\cruise_libft4222.exe`. This can be run from the command line. 

Note: Necessary DLLs are copied into the `build\Debug` directory to enable the executable to run.

## Visual Studio Code

To use this example with Microsoft Visual Studio Code follow the instructions in this web page.

https://code.visualstudio.com/docs/cpp/config-msvc

It is possible to use the command line instructions above in a terminal windows from within Visual Studio Code.

## Visual Studio

The Visual Studio project files for the example are found in the `VisualStudio` folder. Program executables and object files are placed into the `VisualStudio\Debug` folder. 

When the example application is launched in Visual Studio it will add the location of the `LibFT4222-64.dll` or `LibFT4222.dll` file to the PATH environment variable of the running process.

## Linux

The cmake command line examples for Windows will compile the required files for the example application under Linux. The LibFT4222 library for Linux will be taken from the `/usr/local/lib` directory and the include files from `/usr/local/include`.
