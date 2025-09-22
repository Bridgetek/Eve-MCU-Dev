# EVE-MCU-Dev for NXPK64

The port for NXPK64 comprises the MCU connected to the EVE device via the SPI bus. The NXPK64 device manages the EVE device SPI bus. 

## Hardware

The NXPK64 port was developed using a FRDM K64F. The NXPK64 module can be connected via short wires to the corresponding signals of an EVE module. Please reference the FRDM K64F Datasheet for more information.

| FRDM K64F Name | FRDM K64F Pin | EVE Signal |
| --- | --- | --- |
| SCK | J6 5 | SCK |
| MOSI | J6 6 | MOSI |
| MISO | J6 7 | MISO |
| CS Port D4 | J6 4 | CS# |
| Port B20 | J6 3 | PD# |
| 5V | J3 10 | 5V |
| GND | J6 1 | GND |

Ensure that the power supply from the FRDM K64F module is capable of also powering the EVE board. If using third-party modules which may consume more current, a separate power connection to the EVE module could be used, with the grounds of the FRDM K64F and EVE modules common to both power sources.
