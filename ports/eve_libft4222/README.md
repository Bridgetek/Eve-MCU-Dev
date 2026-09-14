# EVE-MCU-Dev Ports for libft4222

[Back](../README.md)

The port for libft4222 comprises a host PC with a USB host controller connecting to the target EVE device through an FTDI FT4222H device. The FT4222H device manages the EVE device SPI bus. The host PC uses a library or DLL to manage the interface with the FT4222H device.

## Hardware

This code requires an FT4222H adapter to connect to the EVE module. The FT4222H adapter is normally based around the [FTDI FT4222H](https://ftdichip.com/products/ft4222h/) USB bridging chip.

#### UMFT4222EV-D

A popular solution which makes it easy to get up and running quickly is the FTDI [UMFT4222EV-D](https://ftdichip.com/products/umft4222ev/) module. 

The UMFT4222EV-D module uses the FT4222H chipset. The module can be connected to the associated pins of the EVE module using jumper wires.

| UMFT4222EV-D Name | UMFT4222EV-D Pin | EVE Signal |
| --- | --- | --- |
| SCK | JP5 Pin 9 | SCK |
| MOSI | JP5 Pin 7 | MOSI |
| MISO | JP5 Pin 8 | MISO |
| SSO0 | JP4 Pin 9 | CS# |
| GPIO2 | JP4 Pin 6 | INT# _(1)_ |
| GPIO0 | JP4 Pin 4 | PD# |
| VCC (5V) | JP4 Pin 1 | 5V |
| GND | JP4 Pin 3 / JP5 Pin 10 | GND |

- (1) The INT# line is not required for operation unless `EVE_COPRO_METHOD` macro is set with `EVE_COPRO_INT` in the configuration for EVE-MCU-Dev.

The adapter also provides a 5V power supply via the 5V pin on the header. Ensure that the computer has sufficient power to supply the EVE module when the backlight is on (and if the audio amplifier is used). If the PC cannot provide sufficient power, a separate power connection to the EVE module could be used, with the grounds of the UMFT4222EV-D and the separate power source common together to EVE. 

#### MM4222-QSPI

Bridgetek offer the [MM4222-QSPI](https://brtchip.com/product/mm4222-qspi/) module, designed to support a range of host interfaces for Bridgetek EVE modules, including USB to SPI/QSPI. 

The MM4222-QSPI module uses the FT4222H chipset. The module can be connected to the associated pins of the EVE module using jumper wires. It supports EVE module SPI/QSPI connections through a 10-pin / 2-pin (**CN2** / **CN3**) single-row socket or a 8x2 dual-row socket (**CN8**).

| MM4222-QSPI Name | CN2 pin | CN3 pin | CN8 pin | EVE Signal |
| --- | --- | --- | --- | --- |
| EVE_SCK | 1 | N/A | 1 | SCK |
| EVE_MOSI | 2 | N/A | 4 | MOSI |
| EVE_MISO | 3 | N/A | 3 | MISO |
| EVE_IO2 | N/A | 1 | 6 | I02 |
| EVE_IO3 | N/A | 2 | 5 | IO3 |
| EVE_CS# | 4 | N/A | 2 | CS# |
| EVE_INT# | 5 | N/A | 14 | INT# _(1)_ |
| EVE_RST# | 6 | N/A | 13 | PD# |
| VCC5V-Board | 7 | N/A | 10  | 5V |
| GND | 9 / 10 | N/A | 11 / 12 | GND |

- (1) The INT# line is not required for operation unless `EVE_COPRO_METHOD` macro is set with `EVE_COPRO_INT` in the configuration for EVE-MCU-Dev.

The adapter also provides a 5V power supply via the 5V pin on the header. Ensure that the computer has sufficient power to supply the EVE module when the backlight is on (and if the audio amplifier is used). If the PC cannot provide sufficient power, a separate power connection to the EVE module could be used, with the grounds of the MM4222-QSPI and the separate power source common together to EVE. 


### ME812AU-WH50R Module

The [ME812AU-WH50R](http://brtchip.com/product/me812au-wh50r/) from Bridgetek has an integrated FT4222H and a FT812Q. The screen resolution is 800x480.

The settings required in `EVE_config.h` are:
```c
#define EVE_DEVICE FT812
#define EVE_DISPLAY_RES WVGA
```

### VM816CU50A-D Module

The [VM816CU50A-D](https://brtchip.com/product/vm816cu50a-d/) from Bridgetek has an integrated FT4222H and a BT816Q. The screen resolution is 800x480.

The settings required in `EVE_config.h` are:
```c
#define EVE_DEVICE BT816
#define EVE_DISPLAY_RES WVGA
```

### ME817EV Module

The [ME817EV](https://brtchip.com/product/me817ev/) from Bridgetek has an integrated FT4222H and a BT817Q.

The settings required in `EVE_config.h` are:
```c
#define EVE_DEVICE BT817
```

With an appropiate `EVE_DISPLAY_RES` settings for the attached LCD panel.

### VM820C

The [VM820C](https://brtchip.com/product/vm820c/) Credit Card Module from Bridgetek has an integrated FT4222H and functions as the baseboard for development modules based on the **BT820B** IC.

The settings required in `EVE_config.h` are:
```c
#define EVE_DEVICE BT820
```

With an appropiate `EVE_DISPLAY_RES` settings for the attached LCD panel.

## Windows

Important Information about Windows builds.

To compile this you will have to download the **LibFT4222** "middleware library" for Windows. The recommended version is v1.4.8 or later. The distributed files for this library are collected in the repository by Bridgetek for convenience: 

https://github.com/Bridgetek/FTDI-SW-Release/

This is used as a submodule for Windows compilations and is found in the directory `ports/external/ftdi_libraries`. Ensure that the repository has been cloned before compiling libft4222 builds.

## Linux

Important Information about Linux builds.

To compile this you will have to download the **LibFT4222** "middleware library" for Linux. The recommended version is v1.4.7 or later. It is available from the FTDI website:

https://ftdichip.com/software-examples/ft4222h-software-examples/

Download the latest version of the LibFT4222 library distribution. The file will typically have a name in the format `LibFT4222-linux-vx.x.x.yyy.zip` where *vx.x.x* is the version number. 

The library is installed *once* into the Linux file system. When building the example code the library files (H, DLL and LIB files) are found by the operating system.

### Install the LibFT4222 Middleware Library in Linux

The LibFT4222 library distribution zip file must be extracted into a new directory. The new directory will have a file called `ReadMe.txt` which describes in detail how to install the library on a Linux system. 

Once the library is installed then the build will search the standard locations for include and library files.
