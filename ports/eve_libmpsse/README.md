# EVE-MCU-Dev Ports for libmpsse

[Back](../README.md)

The port for libmpsse comprises a host PC with a USB host controller connecting to the target EVE device through an FTDI FT4232H/FT232H/FT232R device. The FTDI device manages the EVE device SPI bus. The host PC uses a library or DLL to manage the interface with the FTDI device.

## Hardware

This code requires a USB-MPSSE adapter to connect to the EVE module. The USB-MPSSE adapter is normally based around the FTDI FT232H USB bridging chip.

A popular solution which makes it easy to get up and running quickly is the range of C232HM cables from FTDI such as [C232HM-EDHSL](https://ftdichip.com/products/c232hm-edhsl-0/) or [Connective Peripherals High Speed MPSSE Type-C](https://connectiveperipherals.com/products/usb-type-c-high-speed-mpsse).

The C232HM uses the FT232H chipset and has a 50cm cable with ten single pole receptacles. These can be connected to the associated pins of the EVE module.

For the versions with 5V output on the red wire (C232HM-EDHSL and USBC-HS-MPSSE-5V-3.3V500-SPR) the following wires can be used in Table 3.

If the version of the cable is used which has the 3.3V power output (or if using the 5V version but the EVE module requires more current than can be provided) then a separate power connection to the EVE module could be used, with the grounds of the C232HM / USBC-HS-MPSSE and the separate power source common together to EVE.

| Wire colour | EVE Signal |
| --- | --- |
| Orange | SCK |
| Yellow | MOSI |
| Green | MISO |
| Brown | CS# |
| Blue | PD# |
| Red | 5V |
| Black | GND |

An alternative is the VA800A-SPI module can also be used in the same way as the C232HM. *Note that the VA800A-SPI is now discontinued but the information is retained here for reference.*

The VA800A-SPI has a 10-way connector and can be connected directly to the "VM" range of EVE modules such as the VM800B, VM810C50A and VM816C50A. Very short jumper wires can be used to connect it to other EVE modules by connecting the SCK, CS, MOSI, MISO, PD and GND connections. The adapter also provides a 5V power supply via the 5V pin on the header.

| VA800A-SPI | EVE Signal |
| --- | --- |
| CN1-1 | SCK |
| CN1-2 | MOSI |
| CN1-3 | MISO |
| CN1-4 | CS# |
| CN1-5 | INT# |
| CN1-6 | PD# |
| CN1-7 | 5V |
| CN1-8 | N/C |
| CN1-9 | GND |
| CN1-10 | GND |

Ensure that the computer has sufficient power to supply the EVE module when the backlight is on (and if the audio amplifier is used). If the PC cannot provide sufficient power, a separate power connection to the EVE module could be used, with the grounds of the VA800A-SPI and the separate power source common together to EVE. 

## Windows

Important Information about Windows builds.

To compile this you will have to download the **libMPSSE** library for Windows for each example. The recommended version is v1.0.2 or later. It is available from the FTDI website:

https://ftdichip.com/software-examples/mpsse-projects/

Download the latest version of the libMPSSE library distribution. The file will typically have a name in the format `libmpsse-windows-x.x.x.zip` where *x.x.x* is the version number. 

The library is installed *once* into the `ports\eve_libmpsse` directory. When building the example code the library files (H, C and LIB files) are loaded from this location.

### Install the LibMPSSE SPI Middleware Library in EVE-MCU-Dev library

The LibMPSSE library distribution zip file must be extracted into a new directory. The new directory will have a subfolder called `release`. The distribution contains libraries for various CPU architectures. On v1.4.7 these architectures are AMD64 (64-bit Windows) and x86 (32-bit Windows).

Each architecture has:
- A DLL file for LibMPSSE and LIB files for connecting the application to the LibMPSSE DLLs
- A static LibMPSSE LIB file
- C header files for LibMPSSE
- Source code for I2C and SPI layers

A Windows Command Line BAT file `install_libmpsse.bat` is included in this folder. This will copy the correct files from a LibMPSSE library distribution to the 
current directory (`ports\eve_libmpsse`).

To run the BAT file change directory to the `ports\eve_libmpsse` directory. The path to the `release` directory in the extracted distribution folder is passed as the first parameter to the BAT file. For example:

```
> cd ports\eve_libmpsse
> install_libmpsse.bat ..\..\..\libmpsse-windows-1.0.8\release
Installing AMD64 libraries from "..\..\..\libmpsse-windows-1.0.8\release"
Copying "..\..\..\libmpsse-windows-1.0.8\release\build/x64/LIB/libmpsse.lib" to libmpsse.lib
        1 file(s) copied.
Copying "..\..\..\libmpsse-windows-1.0.8\release\include\libmpsse_i2c.h" to libmpsse_i2c.h
        1 file(s) copied.
Copying "..\..\..\libmpsse-windows-1.0.8\release\include\libmpsse_spi.h" to libmpsse_spi.h
        1 file(s) copied.
Copying "..\..\..\libmpsse-windows-1.0.8\release\source\ftdi_common.h" to ftdi_common.h
        1 file(s) copied.
Copying "..\..\..\libmpsse-windows-1.0.8\release\source\ftdi_infra.c" to ftdi_infra.c
        1 file(s) copied.
Copying "..\..\..\libmpsse-windows-1.0.8\release\source\ftdi_infra.h" to ftdi_infra.h
        1 file(s) copied.
Copying "..\..\..\libmpsse-windows-1.0.8\release\source\ftdi_spi.c" to ftdi_spi.c
        1 file(s) copied.
Copying "..\..\..\libmpsse-windows-1.0.8\release\source\ftdi_i2c.c" to ftdi_i2c.c
        1 file(s) copied.
Copying "..\..\..\libmpsse-windows-1.0.8\release\source\ftdi_mid.c" to ftdi_mid.c
        1 file(s) copied.
Copying "..\..\..\libmpsse-windows-1.0.8\release\source\ftdi_mid.h" to ftdi_mid.h
        1 file(s) copied.
Copying "..\..\..\libmpsse-windows-1.0.8\release\source\memcpy.c" to memcpy.c
        1 file(s) copied.
Copying "..\..\..\libmpsse-windows-1.0.8\release\libftd2xx\ftd2xx.h" to ftd2xx.h
        1 file(s) copied.
Copying "..\..\..\libmpsse-windows-1.0.8\release\libftd2xx\WinTypes.h" to WinTypes.h
        1 file(s) copied.
```

Additionally, the `ftd2xx.dll` library is required. This is installed automatically on the system path when Windows installs the driver for an FTDI device.

Due to limitations in the libMPSSE distribution, a small modification is required:

Line 239 of "ftdi_infra.c":
    #else // _WIN32
	    hdll_d2xx = LoadLibrary(L"ftd2xx.dll");
    #endif // _WIN32

Remove the "L" before the DLL name:
    #else // _WIN32
	    hdll_d2xx = LoadLibrary("ftd2xx.dll");
    #endif // _WIN32

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

Important Information about Linux builds.

To compile this you will have to download the **LibMPSSE-SPI** "middleware library" for Linux. The recommended version is v1.0.8 or later. It is available from the FTDI website:

https://ftdichip.com/software-examples/mpsse-projects/libmpsse-spi-examples/

Download the latest version of the LibMPSSE-SPI library distribution. The file will typically have a name in the format `libmpsse-x86_64-x.x.x.tgz` where *x.x.x* is the version number. 

The library is installed *once* into the Linux file system. When building the example code the library files (H, DLL and LIB files) are found by the operating system.

### Install the LibMPSSE-SPI Middleware Library in Linux

The LibMPSSE-SPI library distribution tar file must be extracted into a new directory. The new directory will have a file called `ReadMe.txt` which describes in detail how to install the library on a Linux system. 

Once the library is installed then the build will search the standard locations for include and library files.

The LibMPSSE-SPI library calls the FTDI D2XX driver. The version of FTDI D2XX driver for Linux may be important on certain Linux distributions. Version v1.4.33 has been tested on Ubuntu and is compiled for glibc version "GLIBC\_2.31".  Later versions require a system supporting "GLIBC\_2.34". If the glibc version is not supported then a message such as this will be reported.

```
 sudo ./build/simple_libmpsse
dlopen failed: /lib/x86_64-linux-gnu/libc.so.6: version `GLIBC_2.34' not found (required by /usr/local/lib/libftd2xx.so)
source/ftdi_infra.c:254:Init_libMPSSE(): NULL expression encountered
```

