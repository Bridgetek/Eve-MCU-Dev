# EVE-MCU-Dev Ports for PIC18F

[Back](../README.md)

The port for PIC18F comprises the MCU connected to the EVE device via the SPI bus. The PIC18F device manages the EVE device SPI bus. 

## Hardware

The PIC18F port was developed using a PIC18F46K22. The PIC18F module can be connected via short wires to the corresponding signals of an EVE module. Please reference the PIC18F46K22 Datasheet for more information.

| PIC18F46K22 Pin | EVE Signal |
| --- | --- |
| RC3 | SCK |
| RC5 | MOSI |
| RC4 | MISO |
| RC7 | CS# |
| RC6 | PD# |
| VUSB | 5V |
| GND | GND |

Ensure that the power supply from the PIC18F module is capable of also powering the EVE board. If using third-party modules which may consume more current, a separate power connection to the EVE module could be used, with the grounds of the PIC18F and EVE modules common to both power sources.
