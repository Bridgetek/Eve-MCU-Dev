# EVE-MCU-Dev Ports for libft4222

[Back](../README.md)

The port for libft4222 comprises a host PC with a USB host controller connecting to the target EVE device through an FTDI FT4222H device. The FT4222H device manages the EVE device SPI bus. The host PC uses a library or DLL to manage the interface with the FT4222H device.

## Hardware

This code requires an FT4222H adapter to connect to the EVE module. The FT4222H adapter is normally based around the FTDI FT4222H USB bridging chip.

A popular solution which makes it easy to get up and running quickly is the FTDI [UMFT4222EV-D](https://ftdichip.com/products/umft4222ev/) module. 

The UMFT4222EV-D module uses the FT2222H chipset. The module can be connected to the associated pins of the EVE module using jumper wires.

| UMFT4222EV-D Name | UMFT4222EV-D Pin | EVE Signal |
| --- | --- | --- |
| SCK | JP5 Pin 9 | SCK |
| MOSI | JP5 Pin 7 | MOSI |
| MISO | JP5 Pin 8 | MISO |
| GPIO1 | JP4 Pin 5 | CS# |
| GPIO1 | JP4 Pin 4 | PD# |
| VCC (5V) | JP4 Pin 1 | 5V |
| GND | JP4 Pin 3 / JP5 Pin 10 | GND |

The adapter also provides a 5V power supply via the 5V pin on the header. Ensure that the computer has sufficient power to supply the EVE module when the backlight is on (and if the audio amplifier is used). If the PC cannot provide sufficient power, a separate power connection to the EVE module could be used, with the grounds of the VA800A-SPI and the separate power source common together to EVE. 

### ME812AU-WH50R Module

The ME812AU-WH50R from Bridgetek has an integrated FT4222H and a FT812Q. The screen resolution is 800x480.

The settings required in `EVE_config.h` are:
```
#define FT8XX_TYPE FT812
#define DISPLAY_RES WVGA
```

### VM816CU50A-D Module

The VM816CU50A-D from Bridgetek has an integrated FT4222H and a BT816Q. The screen resolution is 800x480.

The settings required in `EVE_config.h` are:
```
#define FT8XX_TYPE BT816
#define DISPLAY_RES WVGA
```

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
