# EVE-MCU-Dev Ports for Beaglebone

[Back](../README.md)

The port for BeagleBone Black comprises the SBC connected to the EVE device via the SPI bus. The BeagleBone Black device manages the EVE device SPI bus. 

## Requirements 

| Requirement | Details | 
| --- | --- | 
| Host operating system | Linux |
| Application architecture | arm-linux-gnueabihf |
| Build environment | CMake-compatible toolchain, GNU GCC | 
| Platform definition | `PLATFORM_BEAGLEBONE` |
| Other | None |

## Hardware

The BeagleBone Black port was developed using a BeagleBone Black. 

### Connection to EVE

The BeagleBone Black module can be connected via short wires to the corresponding signals of an EVE module. Please reference the BeagleBone Black Datasheet for more information.

The pin connections for Beaglebone modules is as follows for the 2x8 and 1x10 connectors:

| BeagleBone Black Name | BeagleBone Black Pin | 2x8 Pin | 1x10 Pin | EVE Signal | Colour in Figure |
| --- | --- | --- | --- | --- | --- |
| SPI0_SCLK | P9 Pin 22 | 1 | 1 | SCK | Brown 🟫 |
| SPI0_D1 | P9 Pin 18 |  4 | 2 | MOSI | Red 🟥 |
| SPI0_D0 | P9 Pin 21 | 3 | 3 | MISO | Orange 🟧 |
| GPIO 51 | P9 Pin 16 | 2 | 4 | CS# | Yellow 🟨 |
| GPIO 48 | P9 Pin 15 | 13 | 6 | PD# | Blue 🟦 |
| GPIO 50 | P9 Pin 14 | 14 | 5 | INT# _(1)_ | Green 🟩 |
| - | VUSB | 10 | 7 | 5V | Purple 🟪 |
| - | GND | 11/12 | 9/10 | GND | Black ⬛ |

- (1) The INT# line is not required for operation unless `EVE_COPRO_METHOD` macro is set with `EVE_COPRO_INT` in the configuration for EVE-MCU-Dev. **NOTE:** This has not been tested on hardware.

Ensure that the power supply from the BeagleBone Black module is capable of also powering the EVE board. If using third-party modules which may consume more current, a separate power connection to the EVE module could be used, with the grounds of the BeagleBone Black and EVE modules common to both power sources.

## SPI

The following table shows the SPI transfer frequencies for the BeagleBone Black port:

| Initialisation Phase | Runtime |
| --- | --- |
| 1 MHz | 1 MHz |

The SPI bus frequency is not increased for runtime on the BeagleBone Black.
This can be changed if required in an application to suit the application.

The EVE PD# and CS# pins are controlled by software using GPIOs. The CS# and PD# pins are setup as GPIO output pins with a "Low" output and no pull ups or pull downs, as they are controlled by software. 

The INT# pin is setup as a GPIO input with no pull ups or pull downs.

## Files

| File | Function |
| --- | --- |
| [EVE_Linux_BBB.c](EVE_Linux_BBB.c) | MCU layer interface for interface setup, higher level read/writes, endian conversions, PD# pin control, INT# detection. |
| [beaglebone.cmake](beaglebone.cmake) | CMake recipe suitable for including in an application CMakeLists.txt file to include Beaglebone in a project. |

## Compilation

The "platform" macro `PLATFORM_BEAGLEBONE` must be set to enable this port. All other platform macros must be undefined.

For example in the compiler command line the following would be a parameter to the compiler to select the Beaglebone port.

```console
-DPLATFORM_BEAGLEBONE
```

In a CMake recipe the compile definition would be:

```cmake
add_compile_definitions(PLATFORM_BEAGLEBONE)
```

A sample CMake recipe is included which can be used in a project to add the Beaglebone port. 

## Example Software

Please refer to the [BeagleBone Simple example](../../examples/simple/BeagleBone/README.md) for instructions on using the port in an EVE-MCU-Dev project.
