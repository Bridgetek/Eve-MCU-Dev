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

### Install the LibFT4222 Middleware Library in EVE-MCU-Dev library,

The LibFT4222 library distribution zip file must be extracted into a new directory. The new directory will have a subfolder called `imports`. The distribution contains libraries for various CPU architectures. On v1.4.7 these architectures are AMD64 (64-bit Windows) and x86 (32- bit Windows).

Each architecture has:
- A DLL file for LibFT4222
- LIB files for connecting the application to the LibFT4222 DLL and FTD2XX DLLs
- C header files for LibFT4222 and FTD2XX

A Windows Command Line BAT file `install_libft4222.bat` is included in this folder. This will copy the correct files from a LibFT4222 library distribution to the 
current directory (`ports\eve_libft4222`).

To run the BAT file change directory to the `ports\eve_libft4222` directory. The path to the `imports` directory in the extracted distribution folder is passed as the first parameter to the BAT file. For example:

```
> cd ports\eve_libft4222
> install_libft4222.bat ..\..\..\LibFT4222-v1.4.7\imports
Installing AMD64 libraries from "..\..\..\LibFT4222-v1.4.7\imports"
Copying "..\..\..\LibFT4222-v1.4.7\imports\LibFT4222\dll\amd64\LibFT4222-64.dll" to LibFT4222-64.dll
        1 file(s) copied.
Copying "..\..\..\LibFT4222-v1.4.7\imports\LibFT4222\dll\amd64\LibFT4222-64.lib" to libft4222.lib
        1 file(s) copied.
Copying "..\..\..\LibFT4222-v1.4.7\imports\LibFT4222\inc\LibFT4222.h" to libft4222.h
        1 file(s) copied.
Copying "..\..\..\LibFT4222-v1.4.7\imports\ftd2xx\dll\amd64\ftd2xx.lib" to libftd2xx.lib
        1 file(s) copied.
Copying "..\..\..\LibFT4222-v1.4.7\imports\ftd2xx\ftd2xx.h" to ftd2xx.h
        1 file(s) copied.
Copying "..\..\..\LibFT4222-v1.4.7\imports\ftd2xx\WinTypes.h" to WinTypes.h
        1 file(s) copied.
```

The example application will load and use the `LibFT4222-64.dll` or `LibFT4222.dll` file at runtime. This file must be available either locally (in the same directory as the example executable) or on the system path (recommended "C:\Windows\System32"). The `CMakeLists.txt` file in each example will copy the DLL to the same output directory as the executable.

Additionally, the `ftd2xx.dll` library is required. This is installed automatically on the system path when Windows installs the driver for an FTDI device.
