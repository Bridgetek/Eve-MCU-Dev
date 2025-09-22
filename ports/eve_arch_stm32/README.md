# EVE-MCU-Dev Ports for STM32

[Back](../README.md)

The port for STM32 comprises the MCU connected to the EVE device via the SPI bus. The STM32 device manages the EVE device SPI bus. 

## Hardware

The STM32 port was developed using an STM32F0 DISCOVERY. The STM32 module can be connected via short wires to the corresponding signals of an EVE module. Please reference the STM32F0 DISCOVERY Datasheet for more information.

| STM32F0 DISCOVERY Label | EVE Signal |
| --- | --- |
| P93 | SCK |
| P95 | MOSI |
| P94 | MISO |
| P96 | CS# |
| P97 | PD# |
| 5V | 5V |
| GND | GND |

Ensure that the power supply from the STM32F0 DISCOVERY module is capable of also powering the EVE board. If using third-party modules which may consume more current, a separate power connection to the EVE module could be used, with the grounds of the STM32F0 DISCOVERY and EVE modules common to both power sources.

## Ports

There are two separate ports for the STM32:

| Port | PLATFORM macro | Port File |
| --- | --- | --- |
| Keil uVision 5 | PLATFORM_STM32 | [EVE_MCU_STM32.c](EVE_MCU_STM32.c) |
| STM32CubeIDE | PLATFORM_STM32CUBE | [EVE_MCU_STM32CUBE.c](EVE_MCU_STM32CUBE.c) |

