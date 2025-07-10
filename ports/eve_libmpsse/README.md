# EVE-MCU-Dev Examples for libmpsse

The port for libmpsse comprises a host PC with a USB host controller connecting to the target EVE device through an FTDI FT4232H/FT232H/FT232R device. The FTDI device manages the EVE device SPI bus. The host PC uses a library or DLL to manage the interface with the FTDI device.

## Windows

Important Information about Windows builds.

To compile this you will have to download the **libMPSSE** library for Windows for each example. The recommended version is v1.0.2 or later. It is available from the FTDI website:

https://ftdichip.com/software-examples/mpsse-projects/

Download the latest version of the libMPSSE library distribution. The file will typically have a name in the format `libmpsse-windows-x.x.x.zip` where *x.x.x* is the version number. 

The following files are used in this sample code.

- release\build\libmpsse.dll
- release\build\libmpsse.lib
- release\include\libmpsse_i2c.h
- release\include\libmpsse_spi.h
- release\libftd2xx\ftd2xx.h

There may be other, possibly older, versions of this FT8xx-SPI sample code in the "samples" folder of the distribution. 

The example application will load and use the `libmpsse.dll` file. This file must be available either locally (in the same directory as the example application) or on the system path (recommended "C:\Windows\System32").

Due to limitations in the libMPSSE distribution, a small modification is required:

Line 239 of "release\source\ftdi_infra.c":
    #else // _WIN32
	    hdll_d2xx = LoadLibrary(L"ftd2xx.dll");
    #endif // _WIN32

Remove the "L" before the DLL name:
    #else // _WIN32
	    hdll_d2xx = LoadLibrary("ftd2xx.dll");
    #endif // _WIN32

Additionally, the `ftd2xx.dll` library is required. This is installed automatically on the system path when Windows installs the driver for an FTDI device.

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

When the example application is launched in Visual Studio it will add the location of the `libmpsse.dll` file to the PATH environment variable of the running process.

## Linux

The cmake command line examples for Windows will compile the required files for the example application under Linux. The libMPSSE library for Linux will be taken from the `/usr/local/lib` directory and the include files from `/usr/local/include`.
