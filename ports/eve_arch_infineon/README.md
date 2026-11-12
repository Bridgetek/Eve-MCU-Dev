# EVE-MCU-Dev Port for Infineon PSoC 6

[Back](../README.md)

The port for Infineon PSoC 6 comprises the MCU connected to the EVE device via the SPI bus. The Infineon PSoC 6 device manages the EVE device SPI bus. 

This port is selected with the `PLATFORM_INFINEON_PSOC6` macro.

The Infineon ModusToolbox Eclipse IDE is used for compiling the examples. The ModusToolbox handles the BSPs for each version of device. Calls are made to the Infineon ModusToolbox HAL to control the SPI, GPIO and timers.

## Hardware

The Infineon PSoC 6 port directly supports the following modules:
- [CY8CPROTO-062S2-43439](https://www.infineon.com/evaluation-board/CY8CPROTO-062S2-43439)

## Hardware Interfaces

The port will support SPI hardware drivers in Infineon PSoC 6 devices.

The SPI hardware interface on the Infineon PSoC 6 is a single channel SPI. 

The EVE PD# and CS# pins are controlled by software using GPIOs.The CS# and PD# pins are setup as GPIO output pins with a "Low" output and no pull ups or pull downs, as they are controlled by software. 

The INT# pin is setup as a GPIO input with no pull ups or pull downs.

### Connection to EVE

The Infineon PSoC 6 module can be connected via short wires to the corresponding signals of an EVE module. Please reference the Datasheet for more information.

| Infineon PSoC 6 Pin | Infineon PSoC 6 Pin Name | 2x8 Pin | 1x10 Pin | EVE Signal | Colour in Figure |
| --- | --- | --- | --- | --- | --- |
| 5.2 | P5_2 | 1 | 1 | SCK | Brown 🟫 |
| 5.0 | P5_0 | 4 | 2 | MOSI | Red 🟥 |
| 5.1 | P5_1 | 3 | 3 | MISO | Orange 🟧 |
| 5.7 | P5_7 | 2 | 4 | CS# | Yellow 🟨 |
| 5.6 | P5_6 | 14 | 5 | INT# _(1)_ | Green 🟩 |
| 5.5 | P5_5 | 13 | 6 | PD# | Blue 🟦 |
| N/A | VUSB | 10 | 7 | 5V | Purple 🟪 |
| N/A | GND | 11/12 | 9/10 | GND | Black ⬛ |

- (1) The INT# line is not required for operation unless `EVE_COPRO_METHOD` macro is set with `EVE_COPRO_INT` in the configuration for EVE-MCU-Dev.

Ensure that the power supply from the Infineon PSoC 6 module is capable of also powering the EVE board. If using third-party modules which may consume more current, a separate power connection to the EVE module could be used, with the grounds of the Infineon PSoC 6 and EVE modules common to both power sources.
