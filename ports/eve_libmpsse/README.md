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
