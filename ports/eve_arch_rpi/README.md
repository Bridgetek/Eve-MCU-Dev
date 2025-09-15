# EVE-MCU-Dev Ports for Raspberry Pi

[Back](../README.md)

There are two separate ports for Raspberry Pi products:

| Port | PLATFORM macro | Port File |
| --- | --- | --- |
| [RP2040](#hardware-rp2040) | PLATFORM_RP2040 | [EVE_MCU_RP2040.c](EVE_MCU_RP2040.c) |
| [Raspberry Pi](#hardware-raspberry-pi) | PLATFORM_RASPBERRYPI | [EVE_Linux_RPi.c](EVE_Linux_RPi.c) |

## Hardware RP2040

The RP2040 port was developed using an Raspberry Pi pico. The RP2040 module can be connected via short wires to the corresponding signals of an EVE module. Please reference the Raspberry Pi pico Datasheet for more information.

| RP2040 Pin| RP2040 Pin Name | pico Pin | EVE Signal |
| --- | --- | --- | --- |
| 13 | GPIO10 | 14 | SCK |
| 14 | GPIO11 | 15 | MOSI |
| 15 | GPIO12 | 16 | MISO |
| 16 | GPIO13 | 17 | CS# |
| 9 | GPIO07 | 10 | PD# |
| - | - | 40 | 5V |
| - | - | 13 | GND |

Ensure that the power supply from the Raspberry Pi pico module is capable of also powering the EVE board. If using third-party modules which may consume more current, a separate power connection to the EVE module could be used, with the grounds of theRaspberry Pi pico and EVE modules common to both power sources.

### Compilation

There are two compilation methods for pico boards. CMake and Visual Studio Code.

#### CMake

The first is using the Raspberry Pi pico SDK in command line mode. The example repositories with build options for this have a folder called `pico`. Within this folder there is a file called `CMakeLists.txt` to perform the compilation using CMake.

The CMake environment must be configured first with the command:
```
cmake -B build -S .
```
Then the compilation performed using:
```
cmake --build build
```
The compiled UF2 file is found in the `build` directory.

#### Visual Studio Code

This method requires that the Raspberry Pi pico Extension is loaded into the Visual Studio Code and properly configured. Once this is completed then a workspace file in the `pico_vscode` folder of an example can be opened. 

The instructions for compiling and programming the pico can be followed from the Raspberry Pi pico Extension Guide available from the Raspberry Pi website.

## Hardware Raspberry Pi

The Raspberry Pi port was developed using an Raspberry Pi Model B+ SBC. However it is compatible with all of the Raspberry Pi SBCs sharing the same 40-pin GPIO header. Please reference the Raspberry Pi documentation for more information.

| GPIO Header Name | GPIO Header Pin | EVE Signal |
| --- | --- | --- |
| GPIO11(SCLK) | 23 | SCK |
| GPIO10(MOSI) | 19 | MOSI |
| GPIO9(MISO) | 21 | MISO |
| GPIO23 | 22 | CS# |
| GPIO24 | 18 | PD# |
| 5v Power | 2 | 5V |
| Ground | 6 | GND |

Ensure that the power supply from the Raspberry Pi SBC is capable of also powering the EVE board. If using third-party modules which may consume more current, a separate power connection to the EVE module could be used, with the grounds of theRaspberry Pi SBC and EVE modules common to both power sources.

