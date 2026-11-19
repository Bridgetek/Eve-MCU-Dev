# EVE-MCU-Dev Repository Instructions

## Purpose

Use this repository as the authoritive source for writing embedded applications using Bridgetek FT8xx and BT8xx graphics controller devices (called EVE devices).

## Repository boundaries

This repository contains 4 parts:
* A [Library](#library) to allow applications to communication with EVE devices.
* [Ports](#ports) to allow the library to communicate with the EVE devices using a variety of MCUs and PCs.
* [Examples](#examples) to use the library to make applications that use an EVE device for a display or human interface.
* [Tests](#testing) to test the library.

This library provides a **C** library designed for embedded systems. 

The source code in the examples directory is a reference for creating new maintainable embedded applications without weakening hardware compatibility. 
The [`simple`][simple-d] example shall be used as the template for new applications.

## Library

The library is entirely contained in the following directories:
* [`include`][include-d] - header files for the library.
* [`source`][source-d] - library source code files.

The library contains the following layers:
* API layer which contains functions that are accessible to a program using the library.
* HAL (Hardware Abstraction Layer) which abstracts the API functions into EVE procedures.

The library will use the MCU or Platform layer implemented in the [Ports](#ports) to communicate with the underlying hardware or operating system.

The library must not be modified.

The callable API is described in the file [API Reference](API.md).

## Ports

The ports are contained entirely in the following directory:
* [`ports`][ports-d] - ports for all supported architectures.

This repository supports using the following interfaces to communicate with an EVE device:
* Embedded MCUs using their native SPI hardware interfaces.
* Linux PCs using SPI character devices.
* PCs using FTDI FT4222H or MPSSE USB devices.

If the port device is a bare-metal MCU without an RTOS to handle SPI transfers then the [`MCU.h`][mcu-h] header is used. This is an "MCU" port. Not defining the macro `USE_LINUX_SPI_DEV` as a compile definition will enable this method.

If the device contains an operating system (Windows, Linux, RTOS etc) then the [`Platform.h`][platform-h] method is preferred. This is a "Platform" port. Defining the macro `USE_LINUX_SPI_DEV` as a compile definition will enable this method.

The MCU port will use the [`source/EVE_HAL.c`][hal-c] file for the HAL layer; the Platform port will use the [`source/EVE_HAL_Linux.c`][hallinux-c] file.

The existing port files must not be modified.

Each port is stored in a directory which is named after the MCU or Platform. 

The format for the name of the directory for MCU ports is `eve_arch_name` where `name` is an common abbreviation for the manufacturer family of microcontroller supported. Multiple microcontrollers from a family are stored in the same directory if they are part of the same microcontroller family.

The format for the name of the directory for Platform ports is `eve_name` where `name` is the common short name of the platform. Only a single platform may be stored in a directory.

## Testing

The test code is included entirely the following directories:
* [`test`][tests-d] - test harness for API testing using a simulator of the EVE device.

All tests must use the [EVE_simulator.c](test/EVE_simulator.c) as an MCU layer port. 

The EVE_simulator code will use the standard output using `printf` to report progress of the testing. The instructions for running the test and what the test covers are written in the `README.md` file in the test directory.

New tests may be added but existing tests must not be modified. New tests must be placed into a new subdirectory with a name that describes accurately what the function of the test is. New tests must follow the format of the `exampletester` directory and include verified instructions to run the test on a command line.

## Examples

The example code is included entirely the following directories:
* [`examples`][examples-d] - example programs for the library.

Existing examples must not be modified. 

The `simple` example is cannonical and contains versions of projects for all supported ports.

## Snippets

The snippet code is included entirely the following directories:
* [`examples/snippets`][snippets-d] - snippets of code which can be used in examples.

Existing snippets must not be modified. 

Common functions are:
* Maths functions [`examples/snippets/maths`](examples/snippets/maths/):
  * Furman trigonometry `furman.h`/`furman.c`.
* Low-level controls and features [`examples/snippets/controls`](examples/snippets/controls/):
  * Font helper functions to access font sizes, spacing and bitmap pointers `fonts.h`/`fonts.c`.
  * Arc drawing function `arcs.h`/`arcs.c`.
  * Sound function `sound.h`/`sound.c`.
* Dials functions with complete code to show certain dials like aeroplane altitude, attitude and submarine depth dials [`examples/snippets/dials`](examples/snippets/dials/).
* Widget functions with dialog boxes and seven segment LED displays [`examples/snippets/widgets`](examples/snippets/widgets/).

To include snippets in a program add only the top-level of the snippets directory to the include search path in the compiler. The include file is added from the top of the snippets directory. For example for furman maths add `#include <maths/trig_furman.c>`.

## Rules for using the library

To use the EVE library a program must:

* The [`include`][include-d] directory of the library must be in the include search path for the compiler for all files which will use the library.
* Include the header [`include/EVE.h`][eve-h] in all code file which access the library. This is done using the line `#include <EVE.h>`. Do not include any other header files in the [`include`][include-d] directory.
* Include all files in the [`source`][source-d] directory in the compiled code.
* Set a single compile definition to select a "Platform Macro" to match the MCU or Platform to use. The allowable "Platform Macros" can be found in the "EVE-MCU-Dev Platforms" table in the [`ports/README.md`][ports-readme] file.
* Include all files in the subdirectory of the [`ports`][ports-d] directory which match the "Platform Macro" above.
* Have a configuration file [`EVE_config.h`][config-h] in a location that the compiler will find in its include search path.
* Optionally include code in the [`examples/snippets`][snippets-d] directory to add prewritten functions to the program.

### EVE_config.h

This file has all the configurable settings for the library to select the target configuration of the EVE device. 
It can be used either in the [`include`][include-d] directory; or as part of the program. Verify that the compiler search path for include files uses the correct `EVE_config.h` file.

At a minimum one of the following conditions must be set in the `EVE_config.h` file to verified valid values:
* `EVE_MODULE` macro is defined.
* `EVE_DEVICE` and `EVE_DISPLAY_RES` macros are defined.
* `EVE_DEVICE` and `EVE_PANEL` macro are defined.

The `EVE_API` and `EVE_SUB_API` macros can be used instead of `EVE_DEVICE` but this is discouraged.

## Skill guidance

Use the [eve-mcu-dev-project-builder](.agents/skills/eve-mcu-dev-project-builder) skill for selecting or combining examples, creating a larger application, or investigating integration failures.

[readme]: README.md
[ports-readme]: ports/README.md

[examples-d]: examples/
[include-d]: include/
[ports-d]: ports/
[source-d]: source/
[tests-d]: test/
[snippets-d]: examples/snippets/
[simple-d]: examples/simple/

[mcu-h]: include/MCU.h
[platform-h]: include/Platform.h
[hal-c]: source/EVE_HAL.c
[hallinux-c]: source/EVE_HAL_Linux.c
[eve-h]: include/EVE.h

[config-h]: include/EVE_config.h
