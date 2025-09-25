# EVE-MCU-Dev Ports for MSP

[Back](../README.md)

There are two separate ports for Ti MSP products:

| Port | PLATFORM macro | Port File |
| --- | --- | --- |
| [MSP430](#hardware-MSP430) | `PLATFORM_MSP430` | [EVE_MCU_MSP430.c](EVE_MCU_MSP430.c) |
| [MSPM0](#hardware-MSPM0) | `PLATFORM_MSPM0` | [EVE_MCU_MSPM0.c](EVE_MCU_MSPM0.c) |

The port for MSP430 and MSPM0 comprises the MCU connected to the EVE device via the SPI bus. The MSP device manages the EVE device SPI bus. 

## Hardware MSP430

The MSP430 port was developed using a MSP-EXP430G2 LaunchPad Development Kit. The MSP430 module can be connected via short wires to the corresponding signals of an EVE module. Please reference the MSP-EXP430G2 LaunchPad Development Kit Datasheet for more information.

| Pin | EVE Signal |
| --- | --- |
| J1 p1.4 | SCK |
| J1 p1.2 | MOSI |
| J1 p1.1 | MISO |
| J1 p1.5 | CS# |
| J1 p1.3 | PD# |
| J6 VCC | 5V |
| J6 GND | GND |

Ensure that the power supply from the MSP430 module is capable of also powering the EVE board. If using third-party modules which may consume more current, a separate power connection to the EVE module could be used, with the grounds of the MSP430 and EVE modules common to both power sources.

## Hardware MSPM0

The MSPM0 port was developed using a MSPM0 Development Kit. The MSPM0 module can be connected via short wires to the corresponding signals of an EVE module. Please reference the MSPM0 Development Kit Datasheet for more information.

Pins are defined in the example code using the CCS Studio configuration interface. Control pins for EVE are therefore marked as "N/A" in this table.

| Pin | EVE Signal |
| --- | --- |
| N/A | SCK |
| N/A | MOSI |
| N/A | MISO |
| N/A | CS# |
| N/A | PD# |
| VCC | 5V |
| GND | GND |

Ensure that the power supply from the MSPM0 module is capable of also powering the EVE board. If using third-party modules which may consume more current, a separate power connection to the EVE module could be used, with the grounds of the MSPM0 and EVE modules common to both power sources.
