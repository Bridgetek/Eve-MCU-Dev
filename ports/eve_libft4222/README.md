# EVE-MCU-Dev Examples for libft4222

The port for libft4222 comprises a host PC with a USB host controller connecting to the target EVE device through an FTDI FT4222H device. The FT4222H device manages the EVE device SPI bus. The host PC uses a library or DLL to manage the interface with the FT4222H device.

## Windows

Important Information about Windows builds.

To compile this you will have to download the **LibFT4222** "middleware library" for Windows for each example. The recommended version is v1.4.7 or later. It is available from the FTDI website:

https://ftdichip.com/software-examples/ft4222h-software-examples/

Download the latest version of the LibFT4222 library distribution. The file will typically have a name in the format `LibFT4222-vx.x.x.zip` where *vx.x.x* is the version number. 

For the EVE-MCU-Dev examples, the LibFT4222 library zip file must be unzipped into the `libft4222` directory of the example (where the `main.c` file is kept).

The following files from the LibFT4222 library will be used by the example code:

- imports\LibFT4222\dll\amd64\LibFT4222-64.lib
- imports\LibFT4222\dll\amd64\LibFT4222-64.dll
- imports\ftd2xx\dll\amd64\ftd2xx.lib 
- imports\LibFT4222\dll\x86\LibFT4222.lib
- imports\LibFT4222\dll\x86\LibFT4222.dll
- imports\ftd2xx\dll\x86\ftd2xx.lib 
- imports\ftd2xx\ftd2xx.h
- imports\ftd2xx\WinTypes.h
- imports\LibFT4222\inc\LibFT4222\LibFT4222.h

The example application will load and use the `LibFT4222-64.dll` or `LibFT4222.dll` file at runtime. This file must be available either locally (in the same directory as the example executable) or on the system path (recommended "C:\Windows\System32"). Additionally, the `ftd2xx.dll` library is required. This is installed automatically on the system path when Windows installs the driver for an FTDI device.

### Command Line Compilation

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
The executable file is in the build directory. The exact location is defined by the compiler defaults.

### Visual Studio Code

To use this example with Microsoft Visual Studio Code follow the instructions in this web page.

https://code.visualstudio.com/docs/cpp/config-msvc

It is possible to use the command line instructions above in a terminal windows from within Visual Studio Code.

### Visual Studio

The Visual Studio project files for the example are found in the `VisualStudio` folder. Program executables and object files are placed into the `VisualStudio\Debug` folder. 

When the example application is launched in Visual Studio it will add the location of the `LibFT4222-64.dll` or `LibFT4222.dll` file to the PATH environment variable of the running process.

## Linux

The cmake command line examples for Windows will compile the required files for the example application under Linux. The LibFT4222 library for Linux will be taken from the `/usr/local/lib` directory and the include files from `/usr/local/include`.
