Windows
=======

Important Information about Windows builds using Visual Studio and Visual Studio Code.

To compile this you will have to download the libMPSSE library for Windows. Recommended version is v1.0.2 or later. It is available from the FTDI website:

https://ftdichip.com/software-examples/mpsse-projects/

Download the libmpsse-windows-x.x.x.zip file to this directory and unzip it.

The following files are used in this sample code.

- release\build\libmpsse.dll
- release\build\libmpsse.lib
- release\include\libmpsse_i2c.h
- release\include\libmpsse_spi.h
- release\libftd2xx\ftd2xx.h

There may be other, possibly older, versions of this FT8xx-SPI sample code in the "samples" folder of the distribution. 

The example application will load and use the libmpsse.dll file. This file must be available either locally (in the same directory as the example application) or on the system path (recommended "C:\Windows\System32").

Due to limitations in the libMPSSE distribution, a small modification is required:

Line 239 of "release\source\ftdi_infra.c":
    #else // _WIN32
	    hdll_d2xx = LoadLibrary(L"ftd2xx.dll");
    #endif // _WIN32

Remove the "L" before the DLL name:
    #else // _WIN32
	    hdll_d2xx = LoadLibrary("ftd2xx.dll");
    #endif // _WIN32

Command Line Compilation
------------------------

To compile the project with the default C compiler:

    cmake -B build -S .

On Windows, if an MSVC compiler is not the default compiler on your platform then choose a suitable Visual Studio compiler explicitly:

    cmake -B build -S . -G "Visual Studio 17 2022"

On Windows, to compile the project use the MinGW C compiler:

    cmake -B build -S . -G "MinGW Makefiles"

Then to compile:

    cmake --build build

The executable file is in the build directory. The exact location is defined by the compiler defaults.

Visual Studio Code
------------------

To use this example with Microsoft Visual Studio Code follow the instructions in this web page.

https://code.visualstudio.com/docs/cpp/config-msvc

The configuration, build and launch files are found in the ".vscode" folder. 

Open Visual Studio Code using the command "code ." from a "Developer Command Prompt for VS <version>" to ensure that the build environment is setup correctly.

When the example application is launched in Code it will add the location of the libmpsse.dll file to the PATH environment variable of the running process.

Program executables and object files are placed into the ".vscode\build" folder. 

Visual Studio
-------------

The Visual Studio project files for the example are found in the "VisualStudio" folder. Program executables and object files are placed into the "VisualStudio\Debug" folder. 

When the example application is launched in Visual Studio it will add the location of the libmpsse.dll file to the PATH environment variable of the running process.

Linux
=====

The "Makefile" will compile the required files for the example application. The libMPSSE library for Linux will be taken from the "/usr/local/lib" directory and the include files from "/usr/local/include".
