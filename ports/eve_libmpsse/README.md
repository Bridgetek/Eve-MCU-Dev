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
| Purple | INT# _(1)_ |
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
| CN1-5 | INT# _(1)_ |
| CN1-6 | PD# |
| CN1-7 | 5V |
| CN1-8 | N/C |
| CN1-9 | GND |
| CN1-10 | GND |

Ensure that the computer has sufficient power to supply the EVE module when the backlight is on (and if the audio amplifier is used). If the PC cannot provide sufficient power, a separate power connection to the EVE module could be used, with the grounds of the VA800A-SPI and the separate power source common together to EVE. 

- (1) Please note that the INT# line is not supported in the libmpsse port. Therefore there will be a build error for libmpsse when `EVE_COPRO_METHOD` macro is set with `EVE_COPRO_INT` in the configuration for EVE-MCU-Dev.

## Windows

Important Information about Windows builds.

To compile this you will have to download the **libMPSSE** library for Windows. The recommended version is v1.0.9 or later. The distributed files for this library are collected in the repository by Bridgetek for convenience: 

https://github.com/Bridgetek/FTDI-SW-Release/

This is used as a submodule for Windows compilations and is found in the directory `ports/external/ftdi_libraries`. Ensure that the repository has been cloned before compiling libmpsse builds.
## Linux

Important Information about Linux builds.

To compile this you will have to download the **LibMPSSE-SPI** "middleware library" for Linux. The recommended version is v1.0.9 or later. It is available from the FTDI website:

https://ftdichip.com/software-examples/mpsse-projects/libmpsse-spi-examples/

Download the latest version of the LibMPSSE-SPI library distribution. The file will typically have a name in the format `libmpsse-x86_64-x.x.x.tgz` where *x.x.x* is the version number. 

The library is installed *once* into the Linux file system. When building the example code the library files (H, DLL and LIB files) are found by the operating system.

### Install the LibMPSSE-SPI Middleware Library in Linux

The LibMPSSE-SPI library distribution tar file must be extracted into a new directory. The new directory will have a file called `ReadMe.txt` which describes in detail how to install the library on a Linux system. 

Once the library is installed then the build will search the standard locations for include and library files.

The LibMPSSE-SPI library calls the FTDI D2XX driver. The version of FTDI D2XX driver for Linux may be important on certain Linux distributions. Version v1.4.33 has been tested on Ubuntu and is compiled for glibc version "GLIBC\_2.31".  Later versions require a system supporting "GLIBC\_2.34". If the glibc version is not supported then a message such as this will be reported.

```console
 sudo ./build/simple_libmpsse
dlopen failed: /lib/x86_64-linux-gnu/libc.so.6: version `GLIBC_2.34' not found (required by /usr/local/lib/libftd2xx.so)
source/ftdi_infra.c:254:Init_libMPSSE(): NULL expression encountered
```

