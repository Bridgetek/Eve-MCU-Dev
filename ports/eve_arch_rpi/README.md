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

## Hardware Raspberry Pi

The Raspberry Pi port was developed using an Raspberry Pi Model B+ SBC. However it is compatible with all of the Raspberry Pi SBCs sharing the same 40-pin GPIO header. Please reference the Raspberry Pi documentation for more information.

| GPIO Header Name | GPIO Header Pin | EVE Signal |
| --- | --- | --- |
| GPIO11(SCLK) | 23 | SCK |
| GPIO10(MOSI) | 19 | MOSI |
| GPIO9(MISO) | 21 | MISO |
| GPIO25 | 22 | CS# |
| GPIO24 | 18 | PD# |
| 5v Power | 2 | 5V |
| Ground | 20 | GND |

Ensure that the power supply from the Raspberry Pi SBC is capable of also powering the EVE board. If using third-party modules which may consume more current, a separate power connection to the EVE module could be used, with the grounds of the Raspberry Pi SBC and EVE modules common to both power sources.

