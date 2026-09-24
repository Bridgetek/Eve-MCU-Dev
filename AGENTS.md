# EVE-MCU-Dev Agents Guide

This library allows a variety of hardware to communicate with FT8xx and BT8xx graphics controller devices: 
* Embedded MCUs using their native SPI hardware.
* Linux PCs using SPI character devices.
* PCs using FT4222H or MPSSE USB devices.

This library is intended to provide a **C** library for embedded designs.

The library contains the following layers:
* API layer which contains functions that are accessible to a program using the library.
* HAL (Hardware Abstraction Layer) which abstracts the API functions into EVE procedures.
* MCU or Platform layer that communicates with the underlying hardware or operating system.

The library can be ported to new hardware or used to write GUIs (graphical user interfaces) made from multiple drawing commands to the API layer. 

Multiple commands can be combined to make snippets and widgets.

## Port Device Type

If the port device is a bare-metal MCU without an RTOS to handle SPI transfers then the [`MCU.h`][mcu-h] header is used. This is an "MCU" port.

If the device contains an operating system (Windows, Linux, RTOS etc) then the [`Platform.h`][platform-h] method is preferred. This is a "Platform" port. Defining the macro `USE_LINUX_SPI_DEV` as a compile definition will enable this method.

The API does not depend on the port device. The MCU port will use the [`source/EVE_HAL.c`][hal-c] file for the HAL layer; the platform port will use the [`source/EVE_HAL_Linux.c`][hallinux-c] file.

## Structure

The library includes the following directories:

* [`include`][include-d] - header files for the library.
* [`source`][source-d] - library source code files.
* [`ports`][ports-d] - ports for all supported architectures.
* [`test`][tests-d] - test harness for API testing and simulator.
* [`examples`][examples-d] - example programs for the library.

## Using the Library

To include the EVE library a program must:

* Only include the header [`include/EVE.h`][eve-h] in program code to access the library. This is done using the line `#include <EVE.h>`.
* The `include` directory must be in the include search path for the compiler for all files using the library.
* Include all files in the [`source`][source-d] directory must be compiled into the program.
* Set a compile definition for the "Platform Macro" is required to be set for all library files. The allowable Platform Macros can be found in the [`ports/README.md`][ports-readme] file.
* Include all files in the subdirectory of the [`ports`][ports-d] directory which match the "Platform Macro" above.
* Have a configuration file [`EVE_config.h`][config-h] in a location that the compiler will find in it's include search path.
* Optionally include code in the [`examples/snippets`][snippets-d] directory to add prewritten functions to the program.

The ["simple" example][simple-d] is the template for new programs using the library. This has ports for all supported platforms is used for new projects.

### EVE_config.h

This file has all the configurable settings for the library. 
It can be used either in the [`include`][include-d] directory; or as part of the program.

At a minimum the `EVE_DEVICE` and `EVE_DISPLAY_RES` macros must be set. 
The `EVE_API` and `EVE_SUB_API` macros can be used instead of `EVE_DEVICE` but this is discouraged.

### Platform Macros

The platform macro to select the port used in the program must be set as a compile definition.

## Ports

The objective of an port is to connect the common EVE-MCU-Dev library to the target platforms SPI, GPIO and timing facilities, without modifying the API or HAL layers. 

The required features are:

* Blocking SPI reads and writes. These can be in units of 8-bits or a block of up to 16 kB. Mulitple smaller blocks can be used to make a larger block.
* Predictable control over the chip select (CS#) signal to the SPI. 
  * For Do not use the SPI peripheral automatic chip select function unless the whole block can be buffered before sending.
* Two GPIO output lines:
  * One for controlling chip select (CS#) on the SPI bus.
  * Another for setting the power down (PD#) pin to reset the EVE device.
* A timer function for a 20 ms delay and one for a 500 ms delay.
* A millisecond counter for use in detecting timeouts.
* A macro prefixed with `PLATFORM_` to uniquely identify the port.

Optional features are:
* One input GPIO line:
  * An input signal for interrupt signal (INT#) from the EVE device.
* Support for QuadSPI reads and writes.

### Port Required Files

The port must have C files which can be compiled to implement the prototypes in the include files [`MCU.h`][mcu-h] for MCU ports or [`Platform.h`][platform-h] for Platform ports.

All port C files must be guarded by `#if` or `#ifdef` preprocessor conditions with the Platform Macro to prevent the port code being compiled for other platforms.

A port must provide functions as described in the following table. Note the different file names for MCU and Platform ports.

| Group | MCU Ports | Platform Ports | Responsibility |
| --- | --- | --- | --- |
| Lifecycle | `int MCU_Init(void)`, `int MCU_Setup(void)`, `int MCU_Deinit(void)` | `int Platform_Init(void)`, `int Platform_Setup(void)`, `int Platform_Deinit(void)` | Configure, adjust and release the host interface. Return `0` on success and `-1` on failure. |
| Control pins | `MCU_CSlow()`, `MCU_CShigh()`, `MCU_PDlow()`, `MCU_PDhigh()` | `Platform_CSlow()`, `Platform_CShigh()`, `Platform_PDlow()`, `Platform_PDhigh()` | Control CS# independently of the SPI peripheral and PD#. Apply the named physical levels. |
| Interrupt input | `int MCU_Int(void)` | `int Platform_Int(void)` | Return `0` for EVE INT# pin assertion (logic low) and non-zero for deassertion (logic high). |
| Block transfers | `MCU_SPIWrite(const uint8_t *, uint32_t)`, `MCU_SPIRead(uint8_t *, uint32_t)` | `Platform_SPIWrite(const uint8_t *, uint32_t)`, `Platform_SPIRead(uint8_t *, uint32_t)` | Transfer the requested bytes without changing CS# or adding protocol framing. |
| Scalar transfers | `MCU_SPIWrite8()`, `MCU_SPIWrite16()`, `MCU_SPIWrite24()`, `MCU_SPIWrite32()`, `MCU_SPIRead8()`, `MCU_SPIRead16()`, `MCU_SPIRead32()` | `Platform_SPIWrite8()`, `Platform_SPIWrite16()`, `Platform_SPIWrite24()`, `Platform_SPIWrite32()`, `Platform_SPIRead8()`, `Platform_SPIRead16()`, `Platform_SPIRead32()` | Supply the fixed-size SPI transfer operations used by the HAL. |
| Timing | `MCU_Delay_20ms()`, `MCU_Delay_500ms()`, `uint32_t MCU_Time_ms(void)` | `Platform_Delay_20ms()`, `Platform_Delay_500ms()`, `uint32_t Platform_Time_ms(void)` | Supply minimum delays fuctions and an advancing millisecond count. |
| Host to wire byte order | `MCU_htobe16()`, `MCU_htobe32()`, `MCU_htole16()`, `MCU_htole32()` | `Platform_htobe16()`, `Platform_htobe32()`, `Platform_htole16()`, `Platform_htole32()` | Convert host values to the specified byte order. |
| Wire to host byte order | `MCU_be16toh()`, `MCU_be32toh()`, `MCU_le16toh()`, `MCU_le32toh()` |  `Platform_be16toh()`, `Platform_be32toh()`, `Platform_le16toh()`, `Platform_le32toh()` | Convert the specified byte order to host values. |
| Optional SPI transfer width | `int MCU_SetSPIMode(uint8_t mode)` | `int Platform_SetSPIMode(uint8_t mode)` | Select QuadSPI transfer width when `EVE_QSPI_ENABLE` is defined. |

## Snippets

There are prewritten snippet code examples in the `examples/snippets` directory. These can be used for common functions or making widgets.
For example, the `touch.c` and it's header file `touch.h` are commonly used and recommended to handle touchscreen calibration and touch detection.

Common functions are:
* Maths functions [`examples/snippets/maths`](examples/snippets/maths/):
  * Furnam trigonometry `furman.h`/`furman.c`.
* Low-level controls and features [`examples/snippets/controls`](examples/snippets/controls/):
  * Font helper functions to access font sizes, spacing and bitmap pointers `fonts.h`/`fonts.c`.
  * Arc drawing function `arcs.h`/`arcs.h`.
  * Sound function `sound.h`/`sound.h`.
* Dials functions with complete code to show certain dials like aeroplane altitude, attitude and submarine depth dials [`examples/snippets/dials`](examples/snippets/dials/).
* Widget functions with dialog boxes and seven segment LED displays [`examples/snippets/widgets`](examples/snippets/widgets/).

To add to snippets they must be classified an placed in an appropriate directory with similar snippets.

To include snippets in a program add only the top-level of the snippets directory to the include search path in the compiler. The include file is added from the top of the snippets directory. For example for furman maths add `#include <maths/trig_furman.c>`.

## Testing

There is a "Simulator" port file in the [`test`][tests-d] directory. This is a port file that is used for testing and performs no communication to an EVE device, it doesn't have an SPI bus or use GPIO.

There are `yml` files for testing in the [`workflows`][workflow-d] directory.

The Simulator port is used to test the API: `api_test.yml`.

The Simulator port is used to test build the ["simple" example][simple-d]: `example_test.yml`.

Use the Simulator port to test new programs as shown in these workflow files.

[readme]: README.md
[ports-readme]: ports/README.md
[simple-readme]: examples/simple/README.md

[examples-d]: examples/
[include-d]: include/
[ports-d]: ports/
[source-d]: source/
[tests-d]: test/
[snippets-d]: examples/snippets/
[simple-d]: examples/simple/
[workflow-d]: .github/workflows/

[mcu-h]: include/MCU.h
[platform-h]: include/Platform.h
[hal-h]: include/HAL.h
[hal-c]: source/EVE_HAL.c
[hallinux-c]: source/EVE_HAL_Linux.c
[eve-h]: include/EVE.h
[api-c]: source/EVE_API.c

[config-h]: include/EVE_config.h
[defs-h]: include/EVE_defs.h
[settings-h]: include/EVE_settings.h
[debug-h]: include/EVE_debug.h
