# EVE-MCU-Dev Ports for Arduino

[Back](../README.md)

The port for Arduino comprises a sketch to implement the MCU layer connecting to the EVE device via the SPI bus. The Arduino device manages the EVE device SPI bus. 

## Hardware

The Arduino port was developed using an Arduino Zero and Arduino Leonardo. The Arduino module can be connected via short wires to the corresponding signals of an EVE module. Please reference the Arduino Datasheet for more information.

| Arduino Name | Arduino Pin | EVE Signal |
| --- | --- | --- |
| SCLK | ISCP 3 | SCK |
| COPI | ISCP 4 | MOSI |
| CIPO | ISCP 1 | MISO |
| D10 | - | CS# |
| D9 | - | PD# |
| D8 | - | INT# _(1)_ |
| - | ISCP 2 | 5V |
| - | ISCP 6 / GND | GND |

- (1) The INT# line is not required for operation unless `COPROCESSOR_TRANSFER` macro is set with `EVE_TRANSFER_INT` in the configuration for EVE-MCU-Dev.

Ensure that the power supply from the Arduino module is capable of also powering the EVE board. If using third-party modules which may consume more current, a separate power connection to the EVE module could be used, with the grounds of the Beaglebone Black and EVE modules common to both power sources.

An Arduino board can be connected to an EVE board as in the following picture (the INT# line is not shown).

![Arduino Wiring Picture](../../docs/arduino.png)

**NOTE:** The INT# line is not shown connected.

## Software

Please refer to the [Arduino Simple example](../../examples/simple/arduino/README.md) for instructions on using the hardware sketch in an EVE-MCU-Dev project.
