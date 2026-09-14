# EVE-MCU-Dev Ports for Beaglebone

[Back](../README.md)

The port for BeagleBone Black comprises the MCU connected to the EVE device via the SPI bus. The BeagleBone Black device manages the EVE device SPI bus. 

## Hardware

The BeagleBone Black port was developed using a BeagleBone Black. The BeagleBone Black module can be connected via short wires to the corresponding signals of an EVE module. Please reference the BeagleBone Black Datasheet for more information.

| BeagleBone Black Name | BeagleBone Black Pin | EVE Signal |
| --- | --- | --- |
| SPI0_SCLK | P9 Pin 22 | SCK |
| SPI0_D1 | P9 Pin 18 | MOSI |
| SPI0_D0 | P9 Pin 21 | MISO |
| GPIO 51 | P9 Pin 16 | CS# |
| GPIO 48 | P9 Pin 15 | PD# |
| GPIO 50 | P9 Pin 14 | INT# _(1)_ |
| - | VUSB | 5V |
| - | GND | GND |

- (1) The INT# line is not required for operation unless `EVE_COPRO_METHOD` macro is set with `EVE_COPRO_INT` in the configuration for EVE-MCU-Dev. **NOTE:** This has not been tested on hardware.

Ensure that the power supply from the BeagleBone Black module is capable of also powering the EVE board. If using third-party modules which may consume more current, a separate power connection to the EVE module could be used, with the grounds of the BeagleBone Black and EVE modules common to both power sources.

## Software

Please refer to the [BeagleBone Simple example](../../examples/simple/BeagleBone/README.md) for instructions on using the hardware library in an EVE-MCU-Dev project.
