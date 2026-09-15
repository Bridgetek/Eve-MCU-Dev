# EVE-MCU-Dev

This library allows a variety of hardware to communicate with FT8xx and BT8xx graphics controller devices: embedded MCUs using their native SPI hardware; Linux PCs using SPI character devices; PCs using FT4222H or MPSSE USB devices. 

This library is intended to provide a **C** library for embedded designs.


## Contents

- [Overview](#overview)
  - [History](#history)
  - [Scope](#scope)
  - [Prerequisites](#prerequisites)
  - [Quick Start](#quick-start)
- [Software Layers](#software-layers)
  - [Folder Structure](#folder-structure)
    - [Common Library Files](#common-library-files)
    - [Port Files](#port-files)
    - [Example Files](#example-files)
  - [Header Dependency](#header-dependency)
  - [Device and Panel Selection](#device-and-panel-selection)
    - [Device Selection](#device-selection)
    - [Display Panel Selection](#display-panel-selection)
- [Ports](#ports)
- [Example Code](#example-code)
- [Module Connections](#module-connections)
  - [Through-Board 2x8 Pins](#through-board-2x8-pins)
  - [Header 1x10 Pins](#header-1x10-pins)
- [Library Usage](#library-usage)
  - [Initialising EVE](#initialising-eve)
  - [Co-Processor Lists](#co-processor-lists)
  - [Co-Processor Profiling](#co-processor-profiling)
  - [Co-Processor Helpers](#co-processor-helpers)
  - [EVE Display List Commands](#eve-display-list-commands)
  - [EVE Co-processor Commands](#eve-co-processor-commands)
  - [Creating screens and executing commands](#creating-screens-and-executing-commands)
    - [Writing DL Instructions and Co-Processor Commands](#writing-dl-instructions-and-co-processor-commands)
    - [Beginning and Ending Co-Processor Lists](#beginning-and-ending-co-processor-lists)
    - [Simple Co-Processor List](#simple-co-processor-list)
    - [Executing a Single Co-Processor Command](#executing-a-single-co-processor-command)
    - [Large Co-Processor Lists](#large-co-processor-lists)
    - [Profiling the Co-processor List](#profiling-the-co-processor-list)
    - [Limitations in RAM_DL and RAM_CMD](#limitations-in-ram_dl-and-ram_cmd)
    - [Writing RAM_G and RAM_CMD](#writing-ram_g-and-ram_cmd)
    - [Handling Interrupts](#handling-interrupts)
    - [Accessing the INT# line](#accessing-the-int-line)
- [Programming Guides](#programming-guides)

## Overview

This library is designed to facilitate interfacing to an EVE graphics controller across a variety of MCUs and host platforms. 

It is based around a common set of layers provided as C and Header files which have been arranged to ease portability. Each MCU also has its own source file which includes any MCU-specific items. This allows developers to easily port the code to their chosen MCU type. Full code projects are provided for a range of MCUs and additional types of MCU can be targeted using the same principles explained here.

In addition to the library framework, the code also includes the "simple" demo application for each supported MCU and platform. The "simple" example demonstrates touch, custom fonts, and loading a JPG image. Other example applications are included showing different techniques and methods for using the EVE library.

### History

This document and code library are referred to from the following page on the Bridgetek Website [Home / Software Examples / EVE Examples / Portable EVE Library](https://brtchip.com/software-examples/eve-examples-2/). 

This library was previously described by Application Note [BRT_AN_025 EVE Portable MCU Example](https://brtchip.com/wp-content/uploads/2024/04/BRT_AN_025_EVE_Portable_MCU_Example-R.pdf). **This document and the code within this library supersedes BRT_AN_025**.

The BRT_AN_025 application note built upon the framework described in earlier application notes in the EVE for MCUs series including BRT_AN_008 and explained how it could be ported to other MCU platforms in addition to the original PIC MCU. It focused on aspects of making the library easy to use and demonstrating how it can be run on different MCUs.

### Scope

This document covers the following topics:

- The structure of the library.
- The different library layers which are common to all platforms.
- The basic main sample application provided with the library.
- How to modify the settings and the main example code to produce your own application 
- A separate section for each of the target MCUs describing the hardware requirements and any MCU-specific considerations.

**Note 1:** This code is intended to act as a starting point for customers to create their own application rather than being a complete library package. It is necessary that developers of the final application incorporating this library review all layers of the code as part of their product validation. By using any part of this code, the customer agrees to accept full responsibility for ensuring that their final product operates correctly and complies with any operational and safety requirements and accepts full responsibility for any consequences resulting from its use.

**Note 2:** The library functions are intended to perform a basic set-up of the MCU so that the EVE functionality can be demonstrated. The reader must consult the product documentation provided by the manufacturer of their selected MCU, and the Bridgetek documentation for their EVE device, to confirm that their final code and hardware complies with all recommendations, best practices, and specifications, so that reliable operation of the final product can be assured. The information provided in this document and code is not intended to override any information or specifications in the product datasheets.

### Prerequisites

This code can be used on a wide range of MCUs. Key requirements for compatible MCU are listed below:

- Single or Quad SPI Master with SPI Mode 0 capability.
- SPI signals for SCK and MOSI/MISO (or four bi-directional lines for Quad SPI)
- GPIO line or controllable Chip Select signal for device control
- GPIO line for Power Down control
- An optional GPIO input for an interrupt signal from the EVE device.

The SPI host routines must support transfers of *at least one byte* for EVE API levels 1-4 and at least one 32-bit word for EVE API level 5.

The host interface must also provide software control of the EVE chip-select signal, either directly or through a GPIO, so that it can conform to the EVE SPI protocol.

Some of the provided ports require source code modification if the MCU uses a SPI API library which sends a complete buffer of bytes (such as via a DMA transfer) with automatic chip select control. This is out of the scope of this document and sample code. Most MCUs can however be programmed at a level which interacts directly with the SPI hardware registers and GPIO for chip select. 

This library includes several example projects containing an example framework and sample main application for the following MCUs. However, the code can be ported to other MCUs.

### Quick Start

The [Quick Start Guide](QUICKSTART.md) will demonstrate running one of the example files on the EVE Emulator.

## Device API Support

There are multiple generations of EVE devices, these are referred by their API (and for some devices their SUB API) number from the following table:

| Device | API | SUB API |
| --- | --- | --- |
| FT800, FT801 | 1 | N/A |
| FT810, FT811, FT812, FT813 | 2 | 1 |
| BT880, BT881, BT882, BT883 | 2 | 2 |
| BT815, BT816 | 3 | N/A |
| BT817, BT818 | 4 | N/A |
| BT820 | 5 | N/A |

The library is compiled for the API (and where applicable the SUB API) during compilation. The API cannot be selected at runtime.

## Software Layers

The software consists of several layers which are shown below. The different layers are discussed in greater detail in following sections of this document.

- Main Application
- EVE API Layer
- EVE HAL Layer
- MCU Specific or Platform Specific Layer

The library is structured so that higher-level code depends on lower-level interfaces, while MCU and platform implementations remain independent of the public EVE API and HAL interfaces. This allows multiple examples to share the same library architecture across different supported platforms.

```mermaid
flowchart TD

    APP["Application Layer"]

    API["EVE API Layer"]

    HAL["EVE HAL Layer"]

    MCU["MCU / Host Interface Layer"]
    PLATFORM["Platform Interface Layer"]

    MCUPORT["MCU / Host Port Implementation"]
    LINUXPORT["Linux Platform Implementation"]

    APP --> API
    API --> HAL

    HAL --> MCU
    HAL --> PLATFORM

    MCU --> MCUPORT
    PLATFORM --> LINUXPORT
```

The intended architectural dependency direction is from the Application Layer through the EVE API and HAL layers to the MCU or Platform interface, and finally to the platform-specific implementation.

The HAL layer contains EVE-specific protocol handling, while the MCU and Platform layers provide host-specific SPI, GPIO, timing, interrupt, and operating-system functionality.

Lower-level MCU and platform implementations should remain independent of the public EVE API and HAL interfaces.

### Folder Structure

The library is organised into a number of top-level directories. 

- The `include` directory contains the header files for the EVE API, EVE HAL, and MCU-specific interface.
- The `source` directory contains the common EVE API and EVE HAL implementations.
- Platform-specific MCU implementations are located under the `ports` directory and are selected through the build configuration or platform-specific macros.
- Device- and feature-specific functionality that is not part of the common API implementation is separated into `extensions` subdirectories in the `include` and `source` directories, containing the public extension interfaces and their corresponding source implementations.
- Example applications are published in the `examples` folder.
- Code for simulation and test are placed in the `test` folder.

#### Common Library Files

The library common files for the EVE API and EVE HAL are in the source directory. There are separate files for an MCU implementation and for a _Linux-like_ SPI character device implementation. The HAL abstracts the calls from the API layer selecting the lower MCU or Platform layer for interfacing with the hardware.

The file `EVE_HAL.c` is intended for MCU platforms, the file `EVE_HAL_Linux.c` is for _Linux-like_ platforms such as BeagleBone and RPi platforms. Code which uses the MPSSE and FT4222H interfaces will use the simpler `EVE_HAL.c` code.

Contents of the `source` directory:

- **`EVE_API.c`** The programming interface to the library.
- `EVE_HAL.c` Hardware abstraction layer for MCU-style and supported host-interface ports implementing `MCU.h`.
- `EVE_HAL_Linux.c` Hardware abstraction layer for Linux SPI device ports implementing `Platform.h`.

Include files show the layers inherent in the library. The main API can be accessed with just the `EVE.h` header file which will include all the required header files to compile using the EVE API. The configuration for the display panel and other relevant configurable settings is modified in the `EVE_config.h` file. For most applications this is the only file in the library that will need modification.

Contents of the `include` directory:

- **`EVE.h`** Main public header for applications using the library. It includes the
  configured EVE settings, command definitions, register definitions and public API
  declarations required by an application.

- **`EVE_config.h`** Overridable target configuration file. This is the primary
  configuration file for selecting the EVE device, module, panel, display resolution
  and optional library features.

- `EVE_defs.h` Common definitions used by `EVE_config.h`, including supported EVE
  devices, Bridgetek modules and panels, display resolutions, RAM_G sizes and other
  configuration values.

- `EVE_settings.h` Derives the effective library configuration from `EVE_config.h`.
  This includes EVE API and sub-API selection, module and panel expansion, display
  settings, feature support and compatibility handling for deprecated configuration
  macros.

- `EVE_commands.h` Cross-generation display-list command, co-processor command and
  option definitions. Definitions are selected at compile time for the configured
  EVE API.

- `EVE_registers.h` Cross-generation EVE memory-map and register definitions.
  Register addresses are selected at compile time for the configured EVE API.

- `EVE_debug.h` Platform-specific debug output macros. This header is independent of
  the EVE device configuration and selects the appropriate logging mechanism for the
  target platform.

- `HAL.h` Interface between the EVE API layer and the hardware abstraction layer.

- `MCU.h` Interface implemented by MCU-style and host-interface ports used by
  `EVE_HAL.c`. It defines the SPI, GPIO, timing and other host-side functions required
  by the HAL.

- `Platform.h` Interface implemented by Linux SPI device ports used by
  `EVE_HAL_Linux.c`.

**Bold** files are the files with the recommended access points for a program into the library.

Applications should normally include only `EVE.h`.

The remaining headers are intended for library implementation, configuration or port
development. Each header includes the configuration or standard-library headers that
it directly depends upon and should not rely on `EVE.h` having been included first.

`EVE_config.h` may be replaced by an application-specific version by placing the
replacement earlier in the compiler include search path.

Extension-specific functionality is separated from the common EVE API source and header files. Extension header files are located in `include/extensions`, with their corresponding implementations located in `source/extensions`. These files provide functionality which is required only for specific EVE device generations or configurations and can be excluded from projects if they are not required.

* `/source/extensions/bt82x_patch.c` Implementation of the BT82x base patch loader and the additional API commands provided by the base patch for EVE API level 5 devices.
* `/include/extensions/bt82x_patch.h` Definitions and function declarations for the BT82x base patch functionality.
* `/source/extensions/custom_touch_fw.c` Implementation for loading custom touch firmware into supported EVE devices when the `EVE_CUSTOM_TOUCH` define is enabled.
* `/include/extensions/custom_touch_fw.h` Function declarations for the custom touch firmware extension.
* `/source/extensions/lcd_panel_init.c` Implementation for optional LCD panel driver initialisation when the `EVE_LCD_INIT` define is enabled. MCU-specific functionality is required to provide the interface to the LCD panel driver.
* `/include/extensions/lcd_panel_init.h` Function declarations for the LCD panel initialisation extension.

The extension source files are included in the build only where required for the selected EVE API or configuration. Extension headers are referenced through the main `include` directory, for example `#include <extensions/bt82x_patch.h>`.

#### Port Files

The `ports` directory contains a folder for each supported platform.

MCU-style and host-interface ports implement the interface declared by `MCU.h` and are used by `EVE_HAL.c`.

Linux SPI ports implement the interface declared by `Platform.h` and are used by `EVE_HAL_Linux.c`.

These implementations provide the host-specific SPI, GPIO, timing, byte-order and optional interrupt functionality required by the HAL. Where supported, a port may also provide host-side configuration for optional interfaces such as Quad SPI. Port-specific SPI buffer sizes and transfer limits are implementation details and are independent of `EVE_HAL_CHUNK_SIZE`.

EVE-specific register and command handling remains in the HAL layer and should not normally be implemented by the MCU or Platform layer. Likewise, HAL transfer chunking policy remains in the HAL layer, while MCU and Platform implementations manage only the transfer and buffering constraints of their host interface.

It is further discussed in the [Ports](#ports) section.

#### Example Files

The examples directory contains all the examples provided. There are more details in the [Example Code](#example-code) section.

### Header Dependency

The library headers are grouped below by their role within the library structure. The arrows show direct include dependencies between files. Software layers are
shown alongside shared configuration, definition, extension, and utility headers.

The public EVE-MCU-Dev interface headers `EVE.h`, `HAL.h`, `MCU.h`, and `Platform.h` are included through the configured include path using angle brackets. Extension headers under `include/extensions` are also included through the configured include path, for example `<extensions/bt82x_patch.h>`.

Internal support headers such as `EVE_registers.h`, `EVE_commands.h`, and `EVE_debug.h` use quoted includes where they are consumed within the library or port implementations. `EVE_settings.h` is likewise an internal library header. `EVE_config.h` and `EVE_defs.h` are intentionally included using angle brackets so that applications may provide a matching pair of configuration and definition headers through the configured include path.

#### Application Layer
```mermaid
block
  columns 3
  block:APPLICATION[" "]:3
    columns 3
    A1["Application Source Code"] space B1["EVE.h"]
  end
  A1 --> B1
```

#### EVE API Layer
Public API Header and source implementation:
```mermaid
block
  columns 3
  block:HEADER_LAYER[" "]:3
    columns 3
    A1["EVE.h"]      X1((" "))   B1["EVE_settings.h"]
    space            X2((" "))  B2["EVE_commands.h"]
    space            X3((" "))  B3["EVE_registers.h"]
    A1 --- X1
    X1 --- X2
    X2 --- X3
    X1 --> B1
    X2 --> B2
    X3 --> B3
  end
  block:SOURCE_LAYER[" "]:3
    columns 3
    A2["EVE_API.c"]  X4((" "))    B4["EVE.h"]
    space            X5((" "))    B5["HAL.h"]
    A2 --- X4
    X4 --- X5
    X4 --> B4
    X5 --> B5
  end
```

#### Shared Configuration Headers

These headers provide shared compile-time configuration used by multiple library layers and interfaces:

```mermaid
block
  columns 3

  block:CONFIG_LAYER[" "]:3
    columns 3

    A1["EVE_settings.h"]  space   B1["EVE_config.h"]
    A2["EVE_config.h"]    space   B2["EVE_defs.h"]
    A3["EVE_defs.h <br> <br> (Common device module, panel and option definitions)"] 

    A1 --> B1
    A2 --> B2
  end
```

#### EVE Command and Register Definitions
Command encodings and register definitions used by the EVE API and HAL implementations:

```mermaid
block
  columns 3
  block:COMMAND_REGISTER_LAYER[" "]:3
    columns 3

    A1["EVE_commands.h"]    space  B1["EVE_settings.h"]  
    A2["EVE_registers.h"]   space  B2["EVE_settings.h"]

    A1 --> B1
    A2 --> B2
  end
```

#### HAL Layer
Public HAL Header and source implementations for MCU or Linux based platforms:
```mermaid
block
  columns 4
  block:TOP_LAYER[" "]:4
    columns 3
    A1["HAL.h"]     space       B1["EVE_settings.h"]
    A1 --> B1
  end 
  block:MCU_LAYER[" "]:2
    columns 3
    A2["EVE_HAL.c"] X2((" "))   B2["HAL.h"]             
    space           X3((" "))   B3["MCU.h"]             
    space           X4((" "))   B4["EVE_registers.h"]   
    space           X5((" "))   B5["EVE_commands.h"]    
    space           X6((" "))   B6["EVE_debug.h"]       
    A2 --- X2
    X2 --- X3
    X3 --- X4
    X4 --- X5
    X5 --- X6
    X2 --> B2
    X3 --> B3
    X4 --> B4
    X5 --> B5
    X6 --> B6
  end
  block:PLATFORM_LAYER[" "]:2
    columns 3
    A3["EVE_HAL_Linux.c"]  X7((" "))   B7["Platform.h"]
    space                  X8((" "))   B8["MCU.h"] 
    space                  X9((" "))   B9["EVE_registers.h"]
    space                  X10((" "))  B10["EVE_commands.h"]
    space                  X11((" "))  B11["EVE_debug.h"]
    A3 --- X7
    X7 --- X8
    X8 --- X9
    X9 --- X10
    X10 --- X11
    X7 --> B7
    X8 --> B8
    X9 --> B9
    X10 --> B10
    X11 --> B11
  end
```

#### MCU / Platform Interface Layer

Public HAL-facing interfaces with no dependency on EVE.h or HAL.h:

```mermaid
block
  columns 3

  block:MCU_SIDE[" "]:3
    columns 3

    A1["MCU.h"] space B1["EVE_settings.h"]

    A1 --> B1
  end
  block:PLATFORM_SIDE[" "]:3
    columns 3

    A2["Platform.h"] space B2["EVE_settings.h"]

    A2 --> B2
  end
```

#### Port Implementation 
Port specfic source implementations:

```mermaid
block
  columns 3
  block:PORT[" "]:3
    columns 4

    A1["ports/eve_*/EVE_*.c"] X1((" ")) space B1["MCU.h <i>(1)</i>"]
    space                    X2((" ")) space B2["Platform.h <i>(2)</i>"]
    space                    X3((" ")) space B3["EVE_debug.h <i>(3)</i>"]

    A1 --- X1

    X1 --- X2
    X2 --- X3

    X1 --> B1
    X2 --> B2
    X3 --> B3

  end  

  style X1 fill:none,stroke:none
  style X2 fill:none,stroke:none
  style X3 fill:none,stroke:none
```

* _(1)_ Ports using `EVE_HAL.c` through the `MCU.h` interface.
* _(2)_ Ports using `EVE_HAL_Linux.c` through the `Platform.h` interface.
* _(3)_ Where required for individual port implmentations. 


#### Feature and Device-Specific Extensions

Feature- or device-specific code isolated behind its associated configuration or feature guard.

Extensions may use common EVE functionality and, where required, and may contain MCU- or platform-specific implementation code.

```mermaid
block
  columns 3

  block:EXTENSIONS[" "]:3
    columns 3

    A1["source/extensions/*.c"] space B1["include/extensions/*.h"]

    EXAMPLES["<b>Examples</b>"]:3

    C1["custom_touch_fw.c"] space C2["custom_touch_fw.h"]
    D1["lcd_panel_init.c"]  space D2["lcd_panel_init.h"]
    E1["bt82x_patch.c"]     space E2["bt82x_patch.h"]
  end

  style EXAMPLES fill:none,stroke:none
```
#### Independent Debug Utility
Shared debug macro interface with no dependency on EVE.h:

```mermaid
block
  columns 3

  block:DEBUG[" "]:3
    columns 1

    A1["EVE_debug.h"]
  end 
```

#### Configuration Headers

The configuration headers provide a shared foundation:

```mermaid
block
  columns 1
  A1["EVE_defs.h"]
  space
  A2["EVE_config.h"]
  space
  A3["EVE_settings.h"]
  A1 --> A2
  A2 --> A3
```

`EVE_settings.h` is consumed by the API, HAL, MCU, platform, command, and register definition headers as required. This is a shared configuration dependency rather than a dependency on a higher-level software interface.

Lower-level MCU and platform implementation files should not depend on higher-level headers such as `EVE.h` or `HAL.h`. They may, however, depend on `EVE_settings.h` where derived build-time configuration such as the selected EVE API level or QSPI support affects the host interface implementation

`EVE_debug.h` remains independent and may be used by API, HAL, MCU, platform, or port implementation code without requiring `EVE.h`.

Extension code under `include/extensions` and `source/extensions` should depend only on the functionality required by that feature and should remain isolated behind the relevant feature guards. Where required, an extension may contain MCU- or platform-specific implementation code for functionality that is not provided by the common library interfaces.

#### Custom EVE Configuration Headers

`EVE_settings.h` includes `EVE_config.h`, and `EVE_config.h` includes `EVE_defs.h`, using the compiler include search path:

```c
#include <EVE_config.h>
#include <EVE_defs.h>
```

This allows an application to provide custom configuration and definition headers from another include directory. When overriding the library configuration, a compatible `EVE_config.h` and `EVE_defs.h` must both be available in the configured include path.

The two headers should therefore be treated as a matching pair when overriding the library defaults. This prevents a custom `EVE_config.h` from being combined unintentionally with an incompatible library-local `EVE_defs.h`.

### Device and Panel Selection

The library __must__ be compiled for the correct EVE device and panel type. The target EVE device and panel type are defined in the file `EVE_config.h`.

It is **recommended** that the `EVE_config.h` file is modified in a user program by including the modified version before the library version in the search path for include files passed to the compiler.

There are three methods of configuring the EVE device and panel type. 
- The `EVE_DEVICE` macro and `EVE_DISPLAY_RES` macro. (Formerly the `FT8XX_TYPE` macro and `DISPLAY_RES` macro)
  This is the simplest method if a configuration is fixed. The `EVE_MODULE` and `EVE_PANEL` macros may be removed or be set to `EVE_NO_MODULE` and `EVE_NO_PANEL` respectively.
- The `EVE_DEVICE` macro and `EVE_PANEL` macro.
  This sets the `EVE_DISPLAY_RES` for a panel. The `EVE_MODULE` macros may be removed or be set to `EVE_NO_MODULE`.
- A Bridgetek module type may be set. 
  This will configure the `EVE_DEVICE` and `EVE_PANEL` macros. 
  The `EVE_PANEL` macro will be further expanded into a `EVE_DISPLAY_RES` macro.
  
In all cases, the selected module, device and panel settings are resolved in `EVE_settings.h`.

Where `EVE_MODULE` is selected, it determines the corresponding `EVE_DEVICE` and `EVE_PANEL` and may also enable/disable module-specific features.

Where `EVE_PANEL` is selected, it determines the corresponding `EVE_DISPLAY_RES` and may also enable panel-specific features.

`EVE_DISPLAY_RES` is then used to derive the `EVE_DISP_*` timing macro settings used when initialising the EVE display interface.

The `EVE_PANEL` setting is optionally used in the `examples/snippets/touch.c` example snippet to select predefined touchscreen configuration values and bypass calibration. The `EVE_MODULE` setting is also used by `source/extensions/lcd_panel_init.c` to select the appropriate LCD panel driver initialisation sequence where supported.

#### Device and Panel Options

The following options are supported in `EVE_config.h`:

- `EVE_DEVICE` specifies the EVE device type. The following device types are supported:

  | Device Type | Relevant Product |
  | --- | --- | 
  | **EVE_FT800** | [FT800Q](https://brtchip.com/product/ft800/) |
  | **EVE_FT801** | [FT801Q](https://brtchip.com/product/ft801/) |
  | **EVE_FT810** | [FT810Q](https://brtchip.com/product/ft810q-2/) | 
  | **EVE_FT811** | [FT811Q](https://brtchip.com/product/ft811/) |
  | **EVE_FT812** | [FT812Q](https://brtchip.com/product/ft812/) |
  | **EVE_FT813** | [FT813Q](https://brtchip.com/product/ft813/) |
  | **EVE_BT880** | [BT880Q](https://brtchip.com/product/bt880/) |
  | **EVE_BT881** | [BT881Q](https://brtchip.com/product/bt881/) | 
  | **EVE_BT882** | [BT882Q](https://brtchip.com/product/bt882/) | 
  | **EVE_BT883** | [BT883Q](https://brtchip.com/product/bt883/) | 
  | **EVE_BT815** | [BT815Q](https://brtchip.com/product/bt815/) | 
  | **EVE_BT816** | [BT816Q](https://brtchip.com/product/bt816/) | 
  | **EVE_BT817** | [BT817Q](https://brtchip.com/product/bt817q/), [BT817AQ](https://brtchip.com/product/bt817aq/) | 
  | **EVE_BT818** | [BT818Q](https://brtchip.com/product/bt818/) |
  | **EVE_BT820** | [BT820B](https://brtchip.com/product/bt820b/) |

- `EVE_DISPLAY_RES` The resolution of the display panel.
  The following resolutions are defined:
  | Resolution Name | Size | Example |
  | ----- | ----- | ----- |
  | **EVE_QVGA**    | 320 x 240   | [DP-0351-11A](https://brtchip.com/product/dp-0351-11a/) | 
  | **EVE_WQVGA**   | 320 x 240   | [DP-0431-11A](https://brtchip.com/product/dp-0431-11a/), [DP-0502-11A](https://brtchip.com/product/dp-0502-11a/) |
  | **EVE_WQVGAR**  | 480 x 480   | [IDM2040-21R](https://brtchip.com/product/idm2040-21r/) with 2.1-inch round display |
  | **EVE_WVGA**    | 800 x 480   | [DP-0501-01A](https://brtchip.com/product/dp-0501-01a/), [DP-0501-11A](https://brtchip.com/product/dp-0501-11a/), [DP-0701-01A](https://brtchip.com/product/dp-0701-01a/) |
  | **EVE_WSVGA**   | 1024 x 600  | [ME817EV](https://brtchip.com/product/me817ev/) with 7-inch display |
  | **EVE_WXGA**    | 1280 x 800  | [DP-1011-01A](https://brtchip.com/product/dp-1011-01a/) |
  | **EVE_WXGA_NG** | 1280 x 800  | [DP-1011-02A](https://brtchip.com/product/dp-1011-02a/) |
  | **EVE_FULLHD**  | 1920 x 1080 | [DP-1561-01A](https://brtchip.com/product/dp-1561-01a/), [DP-1561-02A](https://brtchip.com/product/dp-1561-02a/) |
  | **EVE_WUXGA**   | 1920 x 1200 | [DP-1012-01A](https://brtchip.com/product/dp-1012-01a/) |
  

- `EVE_PANEL` The Bridgetek panel type of the display panel.
  The following panels are defined:
  | Panel Name | Description | Touch Type |
  | ----- | ----- | ----- |
  | **EVE_DP_0351_11A** | 3.5-inch display panel (**QVGA**) |  Resistive | 
  | **EVE_DP_0431_11A** | 4.3-inch display panel (**WQVGA**) | Resistive  |
  | **EVE_DP_0501_01A** | 5-inch display panel (**WVGA**) | Capacitive  |
  | **EVE_DP_0501_11A** | 5-inch display panel (**WVGA**) | Resistive |
  | **EVE_DP_0502_11A** | 5-inch display panel (**WQVGA**) |  Resistive |
  | **EVE_DP_0701_01A** | 7-inch display panel (**WVGA**) | Capacitive |
  | **EVE_DP_1011_01A** | 10.1-inch display panel (**WXGA**) | Capacitive |
  | **EVE_DP_1011_02A** | 10.1-inch display panel (**WXGA_NG**) | Capacitive |
  | **EVE_DP_1012_01A** | 10.1-inch  display panel (**WUXGA**) |  Capacitive |
  | **EVE_DP_1561_01A** | 15.6-inch display panel (**FullHD**) | Capacitive |
  | **EVE_DP_1561_02A** | 15.6-inch display panel (**FullHD**) | Capacitive |
  | **EVE_DP_IDM43A**   | 4.3-inch display panel (**WQVGA**) | Capacitive |
  | **EVE_DP_IDM21R**   | 2.1-inch display panel (**WQVGAR**) | Capacitive |

- `EVE_MODULE` The Bridgetek module or development kit type for EVE device and display panel. The following options are defined:
  | Module or Kit Name | Description |
  | ----- | ----- |
  | **EVE_VM800B**      | [VM800B35A-BK](https://brtchip.com/product/vm800b35a-bk/) with 3.5-inch display. (**FT800** with **DP-0351-11A**) |
  | **EVE_VM800C35A**   | [VM800C35A-D](https://brtchip.com/product/vm800c35a-d/) with 3.5-inch display. (**FT800** with **DP-0351-11A**) |
  | **EVE_VM800C43A**   | [VM800C43A-D](https://brtchip.com/product/vm800c43a-d/) with 4.3-inch display. (**FT800** with **DP-0431-11A**) |
  | **EVE_VM800C50A**   | [VM800C50A-D]() with 5-inch display. (**FT800** with **DP-0502-11A**) |
  | **EVE_VM810C**      | [VM810C50A-D](https://brtchip.com/product/vm810c50a-d/) with 5-inch display. (**FT810** with **DP-0501-11A**) |
  | **EVE_ME812A**      | [ME812A-WH50R](https://brtchip.com/product/me812a-wh50r/), [ME812AU-WH50R](https://brtchip.com/product/me812au-wh50r/) with 5-inch display. (**FT812** with **DP-0501-11A**) |
  | **EVE_ME813A**      | [ME813A-WH50C](https://brtchip.com/product/me813a-wh50c/) with 5-inch display. (**FT813** with **DP-0501-01A**) |
  | **EVE_VM816C**      | [VM816C50A-D](https://brtchip.com/product/vm816c50a-d/), [VM816CU50A-D](https://brtchip.com/product/vm816cu50a-d/) with 5-inch display. (**BT816** with **DP-0501-11A**) |
  | **EVE_VM880C**      | [VM880C](https://brtchip.com/product/vm880c/) with assumed 4.3-inch display. (**BT880** with **DP-0431-11A**) |
  | **EVE_IDM204021R**  | [IDM2040-21R](https://brtchip.com/product/idm2040-21r/) (**FT800** with 2.1-inch display) |
  | **EVE_IDM204043A**  | [IDM2040-43A](https://brtchip.com/product/idm2040-43a/) (**BT883** with **DP-0431-11A**) |
  | **EVE_IDM20407A**   | [IDM2040-7A](https://brtchip.com/product/idm2040-7a/) (**BT817** with **DP-0701-01A**) |
  | **EVE_VM820B10A**   | [VM820B10A](https://brtchip.com/product/vm820b10a/) with 10.1-inch display. (**BT820** with **DP-1011-02A**) |
  | **EVE_VM820B15A**   | [VM820B15A](https://brtchip.com/product/vm820b15a/) with 15.6-inch display. (**BT820** with **DP-1561-02A**) |
  | **EVE_IDK_FT810_43A**   | [FT810 IC Development Kit](https://brtchip.com/product/idk-ft810-43a/) with 4.3-inch display. (**FT810** with **DP-0431-11A**) |
  | **EVE_IDK_BT816_50A**   | [BT816 IC Development Kit](https://brtchip.com/product/idk-bt816-50a/) with 5.0-inch display. (**BT816** with **DP-0501-11A**) |
  | **EVE_IDK_BT817_70A**   | [BT817 IC Development Kit](https://brtchip.com/product/idk-bt817-70a/) with 7.0-inch display. (**BT817** with **DP-0701-01A**) |
  | **EVE_IDK_BT817_101A**   | [BT817 IC Development Kit](https://brtchip.com/product/idk-bt817-101a/) with 10.1-inch display. (**BT817** with **DP-1011-02A**) |
  | **EVE_IDK_BT820_101A**   | [BT820 IC Development Kit](https://brtchip.com/product/idk-bt820-101a/) with 10.1-inch display. (**BT820** with **DP-1012-01A**) | 

#### Device Selection

The EVE device to target is set in the file `EVE_config.h`, `EVE_settings.h` then maps the selected `EVE_DEVICE` to the corresponding `EVE_API` and, where required, `EVE_SUB_API` to choose the device or the API respectively. One or other of these macros **must** be set correctly for the device being used.

There are predefined settings mapping of device names for `EVE_DEVICE` to `EVE_API`/`EVE_SUB_API` in the EVE API in the library. The [device API table](#device-api-support) can be used to select the correct value of `EVE_DEVICE`.

If the `EVE_DEVICE` macro is used then the "FT" or "BT" part number, above, of the device is set. This line will set a BT820 device and EVE API 5 will be selected automatically.
```c
#define EVE_DEVICE EVE_BT820
```
If `EVE_API` is used this will override any `EVE_DEVICE` values and a number from 1 to 5 is used. For EVE API 2 a subtype of the API is set in the `EVE_SUB_API` macro. So for an FT813 device the following can be used:
```c
#define EVE_API 2
#define EVE_SUB_API 1
```
**The default in the distribution will be a BT817 device**. This is the EVE device used in the [IDK-BT817-70A](https://brtchip.com/product/idk-bt817-70a/) modules.

Note that the example programs will take the `EVE_config.h` file from the `include` directory.

#### Display Panel Selection

The display panel dimensions to use are set in the file `EVE_config.h` using the `EVE_DISPLAY_RES` or `EVE_PANEL` macros.

The macro `EVE_DISPLAY_RES` will enable one of the pre-defined panel settings to be configured with the register values needed for that panel type. The registers are calculated for the standard Bridgetek panels in the resolution indicated by the `EVE_DISPLAY_RES` macro. Other panels may require different register settings. If a new panel is needed then the settings can be derived from the panel specifications or contact Bridgetek Support for advice.

The display panel settings **must** be correct for the panel in used otherwise it is unlikely that there will be any output visible.

**The default in the distribution will be a WVGA panel**. This is the panel used in the [IDK-BT817-70A](https://brtchip.com/product/idk-bt817-70a/) modules.

#### Setting Device and Panel in Build Configuration

The `EVE_MODULE`, `EVE_DEVICE`, `EVE_PANEL` and `EVE_DISPLAY_RES` macros can be set in a build file as a C define. This can be used to change the configuration without editing or changing the `EVE_config.h` file. 

The `EVE_MODULE` macro is parsed first. Setting this to `EVE_NO_MODULE` will allow one or all of the `EVE_DEVICE`, `EVE_PANEL` and `EVE_DISPLAY_RES` macros to be picked up from the build file C definitions. 

Note that the preprocessor may complain if it is asked to change the value of one of the macros. 

## Ports

The supported platforms are listed in the [ports/README.md](ports/README.md) file. 

The source code for each platform is stored in the [ports](ports) directory. Each source file in each ports folder is guarded by one of the PLATFORM_<i>xxx</i> macros, USE_<i>xxx</i> macros, or a development environment specific macro. This way all the files in the ports directory can be loaded into a compiler and ignored if they are not relevant.

## Example Code

There are example projects for each supported platform. The [examples/README.md](examples/README.md) file has details on each of the included examples.

The ["simple"](examples/simple/README.md) example provides build environments for all supported platforms and forms the basis of the other examples provided. Build instructions are included in the ["simple" example directory.](examples/simple/README.md).

## Module Connections

There are 2 standard connectors for EVE modules used by BridgeTek. Alternatively, an MPSSE cable can be used to interface with a host PC via USB.

The connectors can be interfaced with a host MCU using jumper wires. The wiring colours in photographs in this section for each connection are defined in the following table.

| Colour | EVE Signal |
| --- | --- |
| Blue | SCK |
| Green | MOSI |
| Yellow | MISO |
| Orange | CS# |
| Red | PD# |
| Brown | GND |
| Not shown | INT# |

### Through-Board 2x8 Pins

This connector is a through-board connector 2x8 pin with 2.54mm spacing commonly found on the "ME" range of boards. These are designed with longer pins that can be used with the MM900EVxB FT9XX boards to mount the MCU board on top of the EVE module.

| Pin | EVE Signal | Pin | EVE Signal |
| --- | --- | --- | --- |
| 1 | N/C | 2 | N/C |
| 3 | INT# | 4 | PD# |
| 5 | GND | 6 | N/C |
| 7 | 5V | 8 | N/C |
| 9 | N/C | 10 | N/C |
| 11 | N/C | 12 | N/C |
| 13 | MOSI | 14 | MISO |
| 15 | CS# | 16 | SCK |

The 2x8 header can be connected as in the following picture. **NOTE:** The INT# line is not shown connected.

![Wiring for 2x8 Header](docs/header2x8.png)

### Header 1x10 Pins

This connector is the header pin connector 1x10 pin with 2.54mm spacing commonly found on the "VM" range of modules such as the VM800B, VM810C50A and VM816C50A. The connector directly mates with the VA800A-SPI board. 

| Pin | EVE Signal |
| --- | --- |
| 1 | SCK |
| 2 | MOSI |
| 3 | MISO |
| 4 | CS# |
| 5 | INT# |
| 6 | PD# |
| 7 | 5V |
| 8 | N/C |
| 9 | GND |
| 10 | GND |

The 1x10 header can be connected as in the following picture. **NOTE:** The INT# line is not shown connected.

![Wiring for 1x10 Header](docs/header1x10.png)

### MPSSE USB Cables

MPSSE USB cables such as the [MPSSE cables from FTDI](https://ftdichip.com/product-category/products/cables/usb-mpsse-spi-i2c-jtag-master-cable-series/) or [Connective Peripherals High Speed MPSSE Type-C](https://connectiveperipherals.com/products/usb-type-c-high-speed-mpsse) can be used to connect a host PC to an EVE module. 

These cables have wire-ends colour coded as follows.

| Wire colour | EVE Signal |
| --- | --- |
| Orange | SCK |
| Yellow | MOSI |
| Green | MISO |
| Brown | CS# |
| Blue | PD# |
| Red | 5V |
| Black | GND |

## Library Usage

This callable layer is implemented in `EVE.h` and `EVE_API.c` and is called by the main loop of the application. 

Its purpose is to allow the program to use the same syntax as the EVE Programmers Guide when writing to the co-processor and so make programming of the display simpler and more easily maintained. 

The file contains several types of helper function including: 
- Functions which are used to begin, finish and check execution of co-processor lists. 
- Functions for writing data to RAM_G and RAM_CMD.
- A function for calling each Display List instruction and each Co-Processor command from the EVE programmers guide.

### Initialising EVE

#### EVE_Init

Initialise EVE API.

**Detailed description:**

Initialise the EVE API layer, HAL layer and MCU-specific hardware layer.

Before using the library to send instructions to the EVE device the `EVE_Init` function **must** be called.

This function will initialise the GPIO and SPI interface through the port file for the target MCU specific library. It will also write the display settings registers to the values defined in `EVE_config.h`. Note that these **must** be adjusted to suit your display. It then sets up the GPIO and other registers such as PWM (for the backlight) and sound on the EVE device.

A short co-processor list is used to clear the screen.

One additional step performed in `EVE_API.c` is to clear the bitmap handle properties (including BITMAP_LAYOUT_H and BITMAP_SIZE_H). It is important that this code is executed after the GPU is running and rendering the screen and therefore after the REG_PCLK has been set to the required value.

**Returns:**

0 for success or -1 for failure (device not found or unsupported).

**Format:**

`int EVE_Init(void)`

#### EVE_Deinit

De-Initialise EVE API.

**Detailed description:**

This will call the HAL layer and MCU-specific de-initialisation routines.

**Returns:**

0 for success or -1 for failure (device not found or unsupported).

**Format:**

`int EVE_Deinit(void)`

### Co-Processor Lists

These functions perform the necessary tasks to begin and execute co-processor lists.

#### EVE_LIB_BeginCoProList

Begin co-processor list.

| EVE1 | EVE2 | EVE3 | EVE4 | EVE5 |
| ---  | ---  | ---  | ---  | ---  |
| Yes  | Yes  | Yes  | Yes  | Yes  |

**Detailed description:**

Starts a co-processor list. Initialises the API and HAL ready to start
transmitting a co-processor list to the EVE.

This will typically assert chip select to allow the SPI interface to
send data to the EVE.

This must be called before any co-processor commands, display list commands,
and command buffer data is sent to the EVE device.

Certain API commands cannot be sent during a co-processor list as they 
perform standalone functions with EVE RAM_G or registers. These are clearly
marked in "details" section.

**Format:**

`void EVE_LIB_BeginCoProList(void)`

#### EVE_LIB_EndCoProList

End co-processor list.
 
| EVE1 | EVE2 | EVE3 | EVE4 | EVE5 |
| ---  | ---  | ---  | ---  | ---  |
| Yes  | Yes  | Yes  | Yes  | Yes  |

**Detailed Description:** 

Ends a co-processor list. This will perform any operations in the API
and HAL to finish a co-processor list.

This will typically deasserts chip select after updating any registers
on the EVE device that will signal the end of the co-processor list.

This must be called after a call to `EVE_LIB_BeginCoProList` followed by
the co-processor command buffer data.

**Format:**

`void EVE_LIB_EndCoProList(void)`

#### EVE_LIB_AwaitCoProEmpty

Waits for co-processor list to end.

| EVE1 | EVE2 | EVE3 | EVE4 | EVE5 |
| ---  | ---  | ---  | ---  | ---  |
| Yes  | Yes  | Yes  | Yes  | Yes  |

**Detailed Description:**

Will poll the co-processor command list until it has been completed.

This must be called after `EVE_LIB_EndCoProList` for the program to wait for the completion of the co-processor actions.

If the co-processor method is set to `EVE_COPROC_INT` then it will wait for an interrupt signal on the INT# line before testing for correct completion.

**Returns:** 

- 0 for successful completion.
- 0xff for co-processor exception.

**Format:**

`int EVE_LIB_AwaitCoProEmpty(void)`

Waits for the completion of the current commands sent to the co-processor.

#### EVE_LIB_AwaitCoProEmptyTimeout

Waits for co-processor list to end with a millisecond timeout.

| EVE1 | EVE2 | EVE3 | EVE4 | EVE5 |
| ---  | ---  | ---  | ---  | ---  |
| Yes  | Yes  | Yes  | Yes  | Yes  |

**Detailed Description:**

Will poll the co-processor command list until it has been completed or a timeout has occurred.

This must be called after `EVE_LIB_EndCoProList` for the program to wait for the completion of the co-processor actions.

If the co-processor method is set to `EVE_COPROC_INT` then it will wait for an interrupt signal on the INT# line before testing for correct completion.

**Returns:** 

- 0 for successful completion.
- 0xfe for a timeout error.
- 0xff for co-processor exception.

**Format:**

`int EVE_LIB_AwaitCoProEmptyTimeout(uint32_t timeout)`

**Parameters:**

- *timeout* - milliseconds to wait until the function returns with a timeout error.

#### EVE_LIB_RecoverCoPro

Recovers the co-processor in the event of an exception.

| EVE1 | EVE2 | EVE3 | EVE4 | EVE5 |
| ---  | ---  | ---  | ---  | ---  |
| Yes  | Yes  | Yes  | Yes  | Yes  |

**Detailed Description:**

Will reset the co-processor after an exception is reported by `EVE_LIB_AwaitCoProEmpty` or `EVE_LIB_AwaitCoProEmptyTimeout`.

**Format:** 

`void EVE_LIB_RecoverCoPro(void)`

#### EVE_LIB_GetCoProSpace

Returns the space remaining for further commands to be sent to the co-processor.

| EVE1 | EVE2 | EVE3 | EVE4 | EVE5 |
| ---  | ---  | ---  | ---  | ---  |
| Yes  | Yes  | Yes  | Yes  | Yes  |

**Detailed Description:**

Obtains the free space in the co-processor circular buffer. 
This operation may have an effect on the performance of the device.

This must be called during a co-processor list:
after a call to `EVE_LIB_BeginCoProList` and before a call to 
`EVE_LIB_EndCoProList`.

**Returns:**

The number of free bytes in the co-processor circular buffer.

**Format:** 

`uint16_t EVE_LIB_GetCoProSpace(void)`

### Co-Processor Profiling

#### EVE_LIB_BeginCoProProfile

Resets the co-processor list profiling length.

| EVE1 | EVE2 | EVE3 | EVE4 | EVE5 |
| ---  | ---  | ---  | ---  | ---  |
| Yes  | Yes  | Yes  | Yes  | Yes  |

**NOTE:** This is only compiled if the macro `EVE_COPROC_PROFILE` is defined.

**Detailed Description:**

Sets the profiling pointer to zero to restart profiling.

This function can be called at any time.

**Format:** 

`void EVE_LIB_BeginCoProProfile(void)`

#### EVE_LIB_GetCoProProfile

Size of co-processor list since last reset.

| EVE1 | EVE2 | EVE3 | EVE4 | EVE5 |
| ---  | ---  | ---  | ---  | ---  |
| Yes  | Yes  | Yes  | Yes  | Yes  |

**NOTE:** This is only compiled if the macro `EVE_COPROC_PROFILE` is defined.

**Detailed Description:**

Obtains the current profiling pointer for the co-processor list.

This function can be called at any time.

**Returns:** The number of instructions added to the co-processor list since 
the last reset of the profiling pointer.

**Format:** 

`uint16_t EVE_LIB_GetCoProProfile(void)`

#### EVE_LIB_GetDlProfile

Size of display list.

| EVE1 | EVE2 | EVE3 | EVE4 | EVE5 |
| ---  | ---  | ---  | ---  | ---  |
| Yes  | Yes  | Yes  | Yes  | Yes  |

**NOTE:** This is only compiled if the macro `EVE_COPROC_PROFILE` is defined.

**Detailed Description:**

Obtains the current size of the display list.

This function cannot be used within a co-processor list.

**Returns:** The number of instructions instructions currently in the display list.

**Format:** 

`uint16_t EVE_LIB_GetDlProfile(void)`

### Co-Processor Helpers

#### EVE_LIB_Int

Test interrupt input line.

| EVE1 | EVE2 | EVE3 | EVE4 | EVE5 |
| ---  | ---  | ---  | ---  | ---  |
| Yes  | Yes  | Yes  | Yes  | Yes  |

 **Detailed Description:**
 
 This function will check the interrupt input INT# from the EVE device.
 
This function can be called at any time.

**Returns:**

- zero if there is no interrupt.
- >0 if the EVE device is asserting an interrupt.
- -1 if the MCU or Platform does not support reading the interrupt line.

**Format:** 

`int EVE_LIB_Int(void)`

#### EVE_LIB_GetInterrupt

Test if an interrupt flag is set.
 
| EVE1 | EVE2 | EVE3 | EVE4 | EVE5 |
| ---  | ---  | ---  | ---  | ---  |
| Yes  | Yes  | Yes  | Yes  | Yes  |

**NOTE:** This is only compiled if the co-processor method is set to `EVE_COPROC_INT`.

**Detailed Description:**

Will read the interrupt flag register `REG_INT_FLAGS` and add any newly pending to
a status value. The flag register will clear any pending interrupt
when read so the cumulative flagged bits are kept until they are
cleared by the mask in this function.

This function cannot be used within a co-processor list.

**Returns:**

- 0 for no interrupts in the mask being set.
- if any interrupts are set then the return value will contain bits set from the mask parameter.

**Format:** 

`uint8_t EVE_LIB_GetInterrupt(uint8_t mask)`

**Parameters:**

- *mask* - bit mask of interrupts to test. Unmasked interrupts are not modified.

#### EVE_LIB_GetResult

Returns a result from the co-processor command buffer.

| EVE1 | EVE2 | EVE3 | EVE4 | EVE5 |
| ---  | ---  | ---  | ---  | ---  |
| Yes  | Yes  | Yes  | Yes  | Yes  |

**Detailed Description:**

Will return a result value from "offset" words back in the command buffer.
If the value of offset is 1 then the previous value from the co-processor
command buffer is returned.

This function cannot be used within a co-processor list.

**Returns:**

Result of a previous co-processor command.

**Format:** 

`uint32_t EVE_LIB_GetResult(int offset)`

**Parameters:**

- *offset* - number of 32-bit words to go back in the command buffer for the result.

#### EVE_LIB_GetCoProException

Get co-processor exception description.

| EVE1 | EVE2 | EVE3 | EVE4 | EVE5 |
| ---  | ---  | ---  | ---  | ---  |
| *No* | *No* | Yes  | Yes  | Yes  |

**Detailed Description:**

Will query the co-processor exception description to a string.

This function cannot be used within a co-processor list.

**Returns:**

Co-processor exception description. This is a pointer to a string
and must be sufficient to hold 128 characters.

**Format:** 

`void EVE_LIB_GetCoProException(char *desc)`

**Parameters:**

- *desc* - buffer to receive the text of the exception description.

#### EVE_LIB_WriteDataToRAMG

Write a buffer to memory mapped RAM.

| EVE1 | EVE2 | EVE3 | EVE4 | EVE5 |
| ---  | ---  | ---  | ---  | ---  |
| Yes  | Yes  | Yes  | Yes  | Yes  |

**Detailed Description:**

Writes a block of data via SPI to the EVE.

This function cannot be used within a co-processor list.

**Format:** 

`void EVE_LIB_WriteDataToRAMG(const uint8_t *ImgData, uint32_t DataSize, uint32_t DestAddress)`

**Parameters:**

- *ImgData* - pointer to start of data buffer.
- *DataSize* - number of bytes in buffer.
- *DestAddress* - 24-bit/32-bit memory mapped address on EVE.

#### EVE_LIB_ReadDataFromRAMG

Read a buffer from memory mapped RAM.

| EVE1 | EVE2 | EVE3 | EVE4 | EVE5 |
| ---  | ---  | ---  | ---  | ---  |
| Yes  | Yes  | Yes  | Yes  | Yes  |

**Detailed Description:**

Reads a block of data via SPI from the EVE.

This function cannot be used within a co-processor list.

**Format:** 

`void EVE_LIB_ReadDataFromRAMG(uint8_t *ImgData, uint32_t DataSize, uint32_t SrcAddress)`

**Parameters:**

- *ImgData* - pointer to start of receive data buffer.
- *DataSize* - number of bytes to read (rounded up to be 32-bit aligned).
- *DestAddress* - 24-bit/32-bit memory mapped address on EVE.

#### EVE_LIB_WriteDataToCMD

Write a buffer to the co-processor command memory.

| EVE1 | EVE2 | EVE3 | EVE4 | EVE5 |
| ---  | ---  | ---  | ---  | ---  |
| Yes  | Yes  | Yes  | Yes  | Yes  |

**Detailed Description:**

Writes a block of data via SPI to the EVE co-processor.

This must be part of a co-processor list. It will typically be called
after a co-processor command to provide data for the operation.
The data will be added to the co-processor command list therefore the
write will block on available space in this list.

**Format:** 

`void EVE_LIB_WriteDataToCMD(const uint8_t *ImgData, uint32_t DataSize)`

**Parameters:**

- *ImgData* - pointer to start of data buffer.
- *DataSize* - number of bytes in buffer.

#### EVE_LIB_SendString

Write a string the co-processor command memory.

| EVE1 | EVE2 | EVE3 | EVE4 | EVE5 |
| ---  | ---  | ---  | ---  | ---  |
| Yes  | Yes  | Yes  | Yes  | Yes  |

**Detailed Description:**

Writes a string via SPI to the EVE co-processor.

This must be part of a co-processor list. It will typically be called
after a co-processor command to provide a string for the operation.
The data will be added to the co-processor command list therefore the
write will block on available space in this list.

**Returns:**

- size - The number of bytes of string written to the co-processor list. 
This is rounded up to the next 32-bit boundary.

**Format:** 

`uint16_t EVE_LIB_SendString(const char* string)`

**Parameters:**

- *string* - string to be sent to the co-processor memory.

#### EVE_LIB_GetProps

Get properties of an CMD_LOADIMAGE operation.

| EVE1 | EVE2 | EVE3 | EVE4 | EVE5 |
| ---  | ---  | ---  | ---  | ---  |
| Yes  | Yes  | Yes  | Yes  | Yes  |

**Detailed Description:**

Obtains the details of an image decoded by the CMD_LOADIMAGE
co-processor command. The properties of the image are taken from
the co-processor command list.

This function cannot be used within a co-processor list.

**Format:** 

`void EVE_LIB_GetProps(uint32_t *addr, uint32_t *width, uint32_t *height)`

**Parameters:**

- *addr* - pointer to variable to receive the image start address.
- *width* - pointer to variable to receive the image width.
- *height* - pointer to variable to receive the image height.

#### EVE_LIB_GetPtr

Get current allocation pointer.

| EVE1 | EVE2 | EVE3 | EVE4 | EVE5 |
| ---  | ---  | ---  | ---  | ---  |
| Yes  | Yes  | Yes  | Yes  | Yes  |

**Detailed Description:**

Obtains the automatic allocation pointer of the last address
used for certain co-processor operations.

This function cannot be used within a co-processor list.

**Format:** 

`void EVE_LIB_GetPtr(uint32_t *addr)`

**Parameters:**

- *addr* - last allocation address rounded up to the next 32-bit 
boundary.

#### EVE_LIB_GetMatrix

Get the touchscreen transformation matrix.

| EVE1 | EVE2 | EVE3 | EVE4 | EVE5 |
| ---  | ---  | ---  | ---  | ---  |
| Yes  | Yes  | Yes  | Yes  | Yes  |

**Detailed Description:**

Obtains the transformation matrix from a CMD_CALIBRATE operation.

This function cannot be used within a co-processor list.

**Format:**

`void EVE_LIB_GetMatrix(uint32_t *a, uint32_t *b, uint32_t *c, uint32_t *d, uint32_t *e, uint32_t *f)`

**Parameters:**

- *a* -  pointer of variable to receive matrix a.
- *b* -  pointer of variable to receive matrix b.
- *c* -  pointer of variable to receive matrix c.
- *d* -  pointer of variable to receive matrix d.
- *e* -  pointer of variable to receive matrix e.
- *f* -  pointer of variable to receive matrix f.

#### EVE_LIB_MemCrc

Calculate the CRC of a memory area.

| EVE1 | EVE2 | EVE3 | EVE4 | EVE5 |
| ---- | ---- | ---- | ---- | ---- |
| Yes  | Yes  | Yes  | Yes  | Yes  |

**Detailed Description:**

Obtains the CRC of a memory area.

This function cannot be used within a co-processor list.

**Format:**

`void EVE_LIB_MemCrc(uint32_t ptr, uint32_t num, uint32_t *result)`

**Parameters:**

- *ptr* - start of memory area.
- *num* - number of bytes to CRC.
- *result* - pointer to receive the CRC.

#### EVE_LIB_BitmapTransform

Computes a bitmap transformation matrix.

| EVE1 | EVE2 | EVE3 | EVE4 | EVE5 |
| ---- | ---- | ---- | ---- | ---- |
| *No* | Yes  | Yes  | Yes  | Yes  |

**Detailed Description:**

It computes the transform given three corresponding points in screen space and bitmap space.

This function cannot be used within a co-processor list.

**Format:**

`void EVE_LIB_BitmapTransform( int32_t x0, int32_t y0, int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t tx0, int32_t ty0, int32_t tx1, int32_t ty1, int32_t tx2, int32_t ty2, uint32_t *result )`

**Parameters:**

- *x0, y0* - point 0 screen coordinate, in pixels.
- *x1, y1* - point 1 screen coordinate, in pixels.
- *x2, y2* - point 2 screen coordinate, in pixels.
- *tx0, ty0* - point 0 bitmap coordinate, in pixels.
- *tx1, ty1* - point 1 bitmap coordinate, in pixels.
- *tx2, ty2* - point 2 bitmap coordinate, in pixels.
- *result* - set to -1 on success, or 0 if it is not possible to find the solution matrix

#### EVE_LIB_GetImage

Get image properties.

| EVE1 | EVE2 | EVE3 | EVE4 | EVE5 |
| ---- | ---- | ---- | ---- | ---- |
| *No* | *No* | *No* | Yes  | Yes  |

**Detailed Description:**

From the last CMD_LOADIMAGE get the address, size, format and palette of the loaded image.

This function cannot be used within a co-processor list.

**Format:**

`void EVE_LIB_GetImage(uint32_t *addr, uint32_t *fmt, uint32_t *width, uint32_t *height, uint32_t *palette)`

**Parameters:**

- *addr* - pointer to variable to receive the address the image was loaded to.
- *fmt* - pointer to variable to receive the format of the loaded image.
- *width* - pointer to variable to receive the width of the loaded image.
- *height* - pointer to variable to receive the height of the loaded image.
- *palette* - pointer to variable to receive the palette of the loaded image.

#### EVE_LIB_RegRead

Read a register.

| EVE1 | EVE2 | EVE3 | EVE4 | EVE5 |
| ---- | ---- | ---- | ---- | ---- |
| *No* | *No* | *No* | *No* | Yes  |

**Detailed Description:**

Reads a register value.

This function cannot be used within a co-processor list.

**Format:**

`void EVE_LIB_RegRead(uint32_t addr, uint32_t *value)`

**Parameters:**

- *addr* - address of register to read.
- *value* - pointer to receive the contents of the register.

#### EVE_LIB_MemWrite8

Write an 8-bit memory location.

| EVE1 | EVE2 | EVE3 | EVE4 | EVE5 |
| ---- | ---- | ---- | ---- | ---- |
| Yes  | Yes  | Yes  | Yes  | *No* |

**Detailed Description:**

Writes a memory location value.

This function cannot be used within a co-processor list.

**Format:**

`void EVE_LIB_MemWrite8(uint32_t addr, uint8_t value)`

**Parameters:**

- *addr* - address of memory location to write.
- *value* - value to write to memory.

#### EVE_LIB_MemWrite16

Write a 16-bit memory location.

| EVE1 | EVE2 | EVE3 | EVE4 | EVE5 |
| ---- | ---- | ---- | ---- | ---- |
| Yes  | Yes  | Yes  | Yes  | *No* |

**Detailed Description:**

Writes a memory location value.

This function cannot be used within a co-processor list.

**Format:**

`void EVE_LIB_MemWrite16(uint32_t addr, uint16_t value)`

**Parameters:**

- *addr* - address of memory location to write.
- *value* - value to write to memory.

#### EVE_LIB_MemWrite32

Write a 32-bit memory location.

| EVE1 | EVE2 | EVE3 | EVE4 | EVE5 |
| ---- | ---- | ---- | ---- | ---- |
| Yes  | Yes  | Yes  | Yes  | Yes  |

**NOTE:** EVE_LIB_MemWrite32 supports only 32-bit aligned writes on EVE5.

**Detailed Description:**

Writes a memory location value.

This function cannot be used within a co-processor list.

**Format:**

`void EVE_LIB_MemWrite32(uint32_t addr, uint32_t value)`

**Parameters:**

- *addr* - address of memory location to write.
- *value* - value to write to memory.

#### EVE_LIB_MemRead8

Read an 8-bit memory location.

| EVE1 | EVE2 | EVE3 | EVE4 | EVE5 |
| ---- | ---- | ---- | ---- | ---- |
| Yes  | Yes  | Yes  | Yes  | *No* |

**Detailed Description:**

Reads a memory location value.

This function cannot be used within a co-processor list.

**Returns:**

- *value* - value read from memory.

**Format:**

`uint8_t EVE_LIB_MemRead8(uint32_t address)`

**Parameters:**

- *addr* - address of memory location to read.

#### EVE_LIB_MemRead16

Read a 16-bit memory location.

| EVE1 | EVE2 | EVE3 | EVE4 | EVE5 |
| ---- | ---- | ---- | ---- | ---- |
| Yes  | Yes  | Yes  | Yes  | *No* |

**Detailed Description:**

Reads a memory location value.

This function cannot be used within a co-processor list.

**Returns:**

- *value* - value read from memory.

**Format:**

`uint16_t EVE_LIB_MemRead16(uint32_t address)`

**Parameters:**

- *addr* - address of memory location to read.
- *value* - pointer to receive the contents of the register.

#### EVE_LIB_MemRead32

Read a 32-bit memory location.

| EVE1 | EVE2 | EVE3 | EVE4 | EVE5 |
| ---- | ---- | ---- | ---- | ---- |
| Yes  | Yes  | Yes  | Yes  | Yes  |

**NOTE:** EVE_LIB_MemRead32 supports only 32-bit aligned reads on EVE5.

**Detailed Description:**

Reads a memory location value.

This function cannot be used within a co-processor list.

**Returns:**

- *value* - value read from memory.

**Format:**

`uint32_t EVE_LIB_MemRead32(uint32_t address)`

**Parameters:**

- *addr* - address of memory location to read.
- *value* - pointer to receive the contents of the register.

#### EVE_CMD

Send a single 32-bit value to the command buffer.

**Detailed Description:**

Sends a command or word of data to the co-processor command buffer.

This function must be used within a co-processor list.

**Format:**

`void EVE_CMD(uint32_t c)`

**Parameters:**

- `c` - 32-bit command or data to send to the command buffer.

### EVE Display List Commands

The EVE display list commands are available in the EVE API.  
They are generally renamed with the prefix of `EVE_`.

Display list commands must be used within a co-processor list.

The following table shows the display list commands which are supported by each generation of EVE with the API name and the "Command Name" used in the Programming Guide for the EVE device. The format and implementation of the commands are described only in the [Programming Guide](#programming-guides). This list is grouped alphabetically.

| API Name               | Command Name       | EVE1 | EVE2 | EVE3 | EVE4 | EVE5 |
| ---------------------- | ------------------ | ---- | ---- | ---- | ---- | ---- |
| EVE_ALPHA_FUNC         | ALPHA_FUNC         | Yes  | Yes  | Yes  | Yes  | Yes  |
| EVE_BEGIN              | CLEAR_COLOR_RGB    | Yes  | Yes  | Yes  | Yes  | Yes  |
| EVE_BITMAP_EXT_FORMAT  | BITMAP_EXT_FORMAT  | *No* | *No* | Yes  | Yes  | Yes  |
| EVE_BITMAP_HANDLE      | BITMAP_HANDLE      | Yes  | Yes  | Yes  | Yes  | Yes  |
| EVE_BITMAP_LAYOUT      | BITMAP_LAYOUT      | Yes  | Yes  | Yes  | Yes  | Yes  |
| EVE_BITMAP_LAYOUT_H    | BITMAP_LAYOUT_H    | *No* | Yes  | Yes  | Yes  | Yes  |
| EVE_BITMAP_SIZE        | BITMAP_SIZE        | Yes  | Yes  | Yes  | Yes  | Yes  |
| EVE_BITMAP_SIZE_H      | BITMAP_SIZE_H      | *No* | Yes  | Yes  | Yes  | Yes  |
| EVE_BITMAP_SOURCE      | BITMAP_SOURCE      | Yes  | Yes  | Yes  | Yes  | Yes  |
| EVE_BITMAP_SOURCE2     | BITMAP_SOURCE2     | *No* | *No* | Yes  | Yes  | *No* |
| EVE_BITMAP_SOURCE_H    | BITMAP_SOURCE_H    | *No* | *No* | *No* | *No* | Yes  |
| EVE_BITMAP_SWIZZLE     | BITMAP_SWIZZLE     | *No* | *No* | Yes  | Yes  | Yes  |
| EVE_BITMAP_TRANSFORM_A | BITMAP_TRANSFORM_A | Yes  | Yes  | Yes  | Yes  | Yes  |
| EVE_BITMAP_TRANSFORM_B | BITMAP_TRANSFORM_A | Yes  | Yes  | Yes  | Yes  | Yes  |
| EVE_BITMAP_TRANSFORM_C | BITMAP_TRANSFORM_A | Yes  | Yes  | Yes  | Yes  | Yes  |
| EVE_BITMAP_TRANSFORM_D | BITMAP_TRANSFORM_A | Yes  | Yes  | Yes  | Yes  | Yes  |
| EVE_BITMAP_TRANSFORM_E | BITMAP_TRANSFORM_A | Yes  | Yes  | Yes  | Yes  | Yes  |
| EVE_BITMAP_TRANSFORM_F | BITMAP_TRANSFORM_A | Yes  | Yes  | Yes  | Yes  | Yes  |
| EVE_BITMAP_ZORDER      | BITMAP_ZORDER      | *No* | *No* | *No* | *No* | Yes  |
| EVE_BLEND_FUNC         | BLEND_FUNC         | Yes  | Yes  | Yes  | Yes  | Yes  |
| EVE_CALL               | CALL               | Yes  | Yes  | Yes  | Yes  | Yes  |
| EVE_CELL               | CELL               | Yes  | Yes  | Yes  | Yes  | Yes  |
| EVE_CLEAR              | CLEAR              | Yes  | Yes  | Yes  | Yes  | Yes  |
| EVE_CLEAR_COLOR        | CLEAR_COLOR        | Yes  | Yes  | Yes  | Yes  | Yes  |
| EVE_CLEAR_COLOR_A      | CLEAR_COLOR_A      | Yes  | Yes  | Yes  | Yes  | Yes  |
| EVE_CLEAR_COLOR_RGB    | CLEAR_COLOR_RGB    | Yes  | Yes  | Yes  | Yes  | Yes  |
| EVE_CLEAR_STENCIL      | CLEAR_STENCIL      | Yes  | Yes  | Yes  | Yes  | Yes  |
| EVE_COLOR              | COLOR              | Yes  | Yes  | Yes  | Yes  | Yes  |
| EVE_COLOR_A            | COLOR_A            | Yes  | Yes  | Yes  | Yes  | Yes  |
| EVE_COLOR_MASK         | COLOR_MASK         | Yes  | Yes  | Yes  | Yes  | Yes  |
| EVE_COLOR_RGB          | COLOR_RGB          | Yes  | Yes  | Yes  | Yes  | Yes  |
| EVE_CLEAR_TAG          | CLEAR_TAG          | Yes  | Yes  | Yes  | Yes  | Yes  |
| EVE_DISPLAY            | DISPLAY            | Yes  | Yes  | Yes  | Yes  | Yes  |
| EVE_END                | END                | Yes  | Yes  | Yes  | Yes  | Yes  |
| EVE_JUMP               | JUMP               | Yes  | Yes  | Yes  | Yes  | Yes  |
| EVE_LINE_WIDTH         | LINE_WIDTH         | Yes  | Yes  | Yes  | Yes  | Yes  |
| EVE_MACRO              | MACRO              | Yes  | Yes  | Yes  | Yes  | Yes  |
| EVE_NOP                | NOP                | *No* | Yes  | Yes  | Yes  | Yes  |
| EVE_PALETTE_SOURCE     | PALETTE_SOURCE     | *No* | Yes  | Yes  | Yes  | Yes  |
| EVE_PALLETE_SOURCE_H   | PALLETE_SOURCE_H   | *No* | *No* | *No* | *No* | Yes  |
| EVE_POINT_SIZE         | POINT_SIZE         | Yes  | Yes  | Yes  | Yes  | Yes  |
| EVE_REGION             | REGION             | *No* | *No* | *No* | *No* | Yes  |
| EVE_RESTORE_CONTEXT    | RESTORE_CONTEXT    | Yes  | Yes  | Yes  | Yes  | Yes  |
| EVE_RETURN             | RETURN             | Yes  | Yes  | Yes  | Yes  | Yes  |
| EVE_SAVE_CONTEXT       | SAVE_CONTEXT       | Yes  | Yes  | Yes  | Yes  | Yes  |
| EVE_SCISSOR_SIZE       | SCISSOR_SIZE       | Yes  | Yes  | Yes  | Yes  | Yes  |
| EVE_SCISSOR_XY         | SCISSOR_XY         | Yes  | Yes  | Yes  | Yes  | Yes  |
| EVE_STENCIL_FUNC       | STENCIL_FUNC       | Yes  | Yes  | Yes  | Yes  | Yes  |
| EVE_STENCIL_MASK       | STENCIL_MASK       | Yes  | Yes  | Yes  | Yes  | Yes  |
| EVE_STENCIL_OP         | STENCIL_OP         | Yes  | Yes  | Yes  | Yes  | Yes  |
| EVE_TAG                | TAG                | Yes  | Yes  | Yes  | Yes  | Yes  |
| EVE_TAG_MASK           | TAG_MASK           | Yes  | Yes  | Yes  | Yes  | Yes  |
| EVE_VERTEX2F           | VERTEX2F           | Yes  | Yes  | Yes  | Yes  | Yes  |
| EVE_VERTEX2II          | VERTEX2II          | Yes  | Yes  | Yes  | Yes  | Yes  |
| EVE_VERTEX_FORMAT      | VERTEX_FORMAT      | *No* | Yes  | Yes  | Yes  | Yes  |
| EVE_VERTEX_TRANSLATE_X | VERTEX_TRANSLATE_X | *No* | Yes  | Yes  | Yes  | Yes  |
| EVE_VERTEX_TRANSLATE_Y | VERTEX_TRANSLATE_Y | *No* | Yes  | Yes  | Yes  | Yes  |

### EVE Co-processor Commands

The EVE co-processor commands are available in the EVE API.  
Like the display list commands they are generally renamed with the prefix of `EVE_` and must be used within a co-processor list.

The following table shows the co-processor commands which are supported by each generation of EVE with the API name and the "Command Name" used in the Programming Guide for the EVE device. The format and implementation of the commands are described only in the [Programming Guide](#programming-guides). This list is grouped by function and API support.

| API Name                 | Command Name         | EVE1 | EVE2 | EVE3 | EVE4 | EVE5 |
| ------------------------ | -------------------- | ---- | ---- | ---- | ---- | ---- |
| EVE_CMD_KEYS             | CMD_KEYS             | Yes  | Yes  | Yes  | Yes  | Yes  |
| EVE_CMD_NUMBER           | CMD_NUMBER           | Yes  | Yes  | Yes  | Yes  | Yes  |
| EVE_CMD_LOADIDENTITY     | CMD_LOADIDENTITY     | Yes  | Yes  | Yes  | Yes  | Yes  |
| EVE_CMD_GAUGE            | CMD_GAUGE            | Yes  | Yes  | Yes  | Yes  | Yes  |
| EVE_CMD_REGREAD          | CMD_REGREAD          | Yes  | Yes  | Yes  | Yes  | Yes  |
| EVE_CMD_GETPROPS         | CMD_GETPROPS         | Yes  | Yes  | Yes  | Yes  | Yes  |
| EVE_CMD_MEMCPY           | CMD_MEMCPY           | Yes  | Yes  | Yes  | Yes  | Yes  |
| EVE_CMD_SPINNER          | CMD_SPINNER          | Yes  | Yes  | Yes  | Yes  | Yes  |
| EVE_CMD_BGCOLOR          | CMD_BGCOLOR          | Yes  | Yes  | Yes  | Yes  | Yes  |
| EVE_CMD_SWAP             | CMD_SWAP             | Yes  | Yes  | Yes  | Yes  | Yes  |
| EVE_CMD_TRANSLATE        | CMD_TRANSLATE        | Yes  | Yes  | Yes  | Yes  | Yes  |
| EVE_CMD_STOP             | CMD_STOP             | Yes  | Yes  | Yes  | Yes  | Yes  |
| EVE_CMD_SLIDER           | CMD_SLIDER           | Yes  | Yes  | Yes  | Yes  | Yes  |
| EVE_CMD_INTERRUPT        | CMD_INTERRUPT        | Yes  | Yes  | Yes  | Yes  | Yes  |
| EVE_CMD_FGCOLOR          | CMD_FGCOLOR          | Yes  | Yes  | Yes  | Yes  | Yes  |
| EVE_CMD_ROTATE           | CMD_ROTATE           | Yes  | Yes  | Yes  | Yes  | Yes  |
| EVE_CMD_MEMWRITE         | CMD_MEMWRITE         | Yes  | Yes  | Yes  | Yes  | Yes  |
| EVE_CMD_SCROLLBAR        | CMD_SCROLLBAR        | Yes  | Yes  | Yes  | Yes  | Yes  |
| EVE_CMD_GETMATRIX        | CMD_GETMATRIX        | Yes  | Yes  | Yes  | Yes  | Yes  |
| EVE_CMD_SKETCH           | CMD_SKETCH           | Yes  | Yes  | Yes  | Yes  | Yes  |
| EVE_CMD_MEMSET           | CMD_MEMSET           | Yes  | Yes  | Yes  | Yes  | Yes  |
| EVE_CMD_GRADCOLOR        | CMD_GRADCOLOR        | Yes  | Yes  | Yes  | Yes  | Yes  |
| EVE_CMD_BITMAP_TRANSFORM | CMD_BITMAP_TRANSFORM | Yes  | Yes  | Yes  | Yes  | Yes  |
| EVE_CMD_CALIBRATE        | CMD_CALIBRATE        | Yes  | Yes  | Yes  | Yes  | Yes  |
| EVE_CMD_INFLATE          | CMD_INFLATE          | Yes  | Yes  | Yes  | Yes  | Yes  |
| EVE_CMD_SETFONT          | CMD_SETFONT          | Yes  | Yes  | Yes  | Yes  | Yes  |
| EVE_CMD_LOGO             | CMD_LOGO             | Yes  | Yes  | Yes  | Yes  | Yes  |
| EVE_CMD_APPEND           | CMD_APPEND           | Yes  | Yes  | Yes  | Yes  | Yes  |
| EVE_CMD_MEMZERO          | CMD_MEMZERO          | Yes  | Yes  | Yes  | Yes  | Yes  |
| EVE_CMD_SCALE            | CMD_SCALE            | Yes  | Yes  | Yes  | Yes  | Yes  |
| EVE_CMD_CLOCK            | CMD_CLOCK            | Yes  | Yes  | Yes  | Yes  | Yes  |
| EVE_CMD_GRADIENT         | CMD_GRADIENT         | Yes  | Yes  | Yes  | Yes  | Yes  |
| EVE_CMD_SETMATRIX        | CMD_SETMATRIX        | Yes  | Yes  | Yes  | Yes  | Yes  |
| EVE_CMD_TRACK            | CMD_TRACK            | Yes  | Yes  | Yes  | Yes  | Yes  |
| EVE_CMD_GETPTR           | CMD_GETPTR           | Yes  | Yes  | Yes  | Yes  | Yes  |
| EVE_CMD_PROGRESS         | CMD_PROGRESS         | Yes  | Yes  | Yes  | Yes  | Yes  |
| EVE_CMD_COLDSTART        | CMD_COLDSTART        | Yes  | Yes  | Yes  | Yes  | Yes  |
| EVE_CMD_DIAL             | CMD_DIAL             | Yes  | Yes  | Yes  | Yes  | Yes  |
| EVE_CMD_LOADIMAGE        | CMD_LOADIMAGE        | Yes  | Yes  | Yes  | Yes  | Yes  |
| EVE_CMD_DLSTART          | CMD_DLSTART          | Yes  | Yes  | Yes  | Yes  | Yes  |
| EVE_CMD_SNAPSHOT         | CMD_SNAPSHOT         | Yes  | Yes  | Yes  | Yes  | Yes  |
| EVE_CMD_SCREENSAVER      | CMD_SCREENSAVER      | Yes  | Yes  | Yes  | Yes  | Yes  |
| EVE_CMD_MEMCRC           | CMD_MEMCRC           | Yes  | Yes  | Yes  | Yes  | Yes  |
| EVE_CMD_TEXT             | CMD_TEXT             | Yes  | Yes  | Yes  | Yes  | Yes  |
| EVE_CMD_BUTTON           | CMD_BUTTON           | Yes  | Yes  | Yes  | Yes  | Yes  |
| EVE_CMD_TOGGLE           | CMD_TOGGLE           | Yes  | Yes  | Yes  | Yes  | Yes  |
| EVE_CMD_CSKETCH          | CMD_CSKETCH          | *No* | Yes  | *No* | *No* | *No* |
| EVE_CMD_SETROTATE        | CMD_SETROTATE        | *No* | Yes  | Yes  | Yes  | Yes  |
| EVE_CMD_MEDIAFIFO        | CMD_MEDIAFIFO        | *No* | Yes  | Yes  | Yes  | Yes  |
| EVE_CMD_SYNC             | CMD_SYNC             | *No* | Yes  | Yes  | Yes  | Yes  |
| EVE_CMD_ROMFONT          | CMD_ROMFONT          | *No* | Yes  | Yes  | Yes  | Yes  |
| EVE_CMD_PLAYVIDEO        | CMD_PLAYVIDEO        | *No* | Yes  | Yes  | Yes  | Yes  |
| EVE_CMD_VIDEOFRAME       | CMD_VIDEOFRAME       | *No* | Yes  | Yes  | Yes  | Yes  |
| EVE_CMD_VIDEOSTART       | CMD_VIDEOSTART       | *No* | Yes  | Yes  | Yes  | Yes  |
| EVE_CMD_SETBASE          | CMD_SETBASE          | *No* | Yes  | Yes  | Yes  | Yes  |
| EVE_CMD_SETBITMAP        | CMD_SETBITMAP        | *No* | Yes  | Yes  | Yes  | Yes  |
| EVE_CMD_SETSCRATCH       | CMD_SETSCRATCH       | *No* | Yes  | Yes  | Yes  | Yes  |
| EVE_CMD_SETFONT2         | CMD_SETFONT2         | *No* | Yes  | Yes  | Yes  | *No* |
| EVE_CMD_SNAPSHOT2        | CMD_SNAPSHOT2        | *No* | Yes  | Yes  | Yes  | *No* |
| EVE_CMD_INFLATE2         | CMD_INFLATE2         | *No* | *No* | Yes  | Yes  | *No* |
| EVE_CMD_CLEARCACHE       | CMD_CLEARCACHE       | *No* | *No* | Yes  | Yes  | *No* |
| EVE_CMD_INTRAMSHARED     | CMD_INTRAMSHARED     | *No* | *No* | Yes  | Yes  | *No* |
| EVE_CMD_VIDEOSTARTF      | CMD_VIDEOSTARTF      | *No* | *No* | Yes  | Yes  | *No* |
| EVE_CMD_ANIMSTART        | CMD_ANIMSTART        | *No* | *No* | Yes  | Yes  | Yes  |
| EVE_CMD_ANIMSTOP         | CMD_ANIMSTOP         | *No* | *No* | Yes  | Yes  | Yes  |
| EVE_CMD_ANIMXY           | CMD_ANIMXY           | *No* | *No* | Yes  | Yes  | Yes  |
| EVE_CMD_ANIMDRAW         | CMD_ANIMDRAW         | *No* | *No* | Yes  | Yes  | Yes  |
| EVE_CMD_ANIMFRAME        | CMD_ANIMFRAME        | *No* | *No* | Yes  | Yes  | Yes  |
| EVE_CMD_APPENDF          | CMD_APPENDF          | *No* | *No* | Yes  | Yes  | Yes  |
| EVE_CMD_ANIMFRAMERAM     | CMD_ANIMFRAMERAM     | *No* | *No* | *No* | Yes  | *No* |
| EVE_CMD_ANIMSTARTRAM     | CMD_ANIMSTARTRAM     | *No* | *No* | *No* | Yes  | *No* |
| EVE_CMD_APILEVEL         | CMD_APILEVEL         | *No* | *No* | *No* | Yes  | *No* |
| EVE_CMD_FONTCACHE        | CMD_FONTCACHE        | *No* | *No* | *No* | Yes  | *No* |
| EVE_CMD_FONTCACHEQUERY   | CMD_FONTCACHEQUERY   | *No* | *No* | *No* | Yes  | *No* |
| EVE_CMD_HSF              | CMD_HSF              | *No* | *No* | *No* | Yes  | *No* |
| EVE_CMD_PCLKFREQ         | CMD_PCLKFREQ         | *No* | *No* | *No* | Yes  | *No* |
| EVE_CMD_RUNANIM          | CMD_RUNANIM          | *No* | *No* | *No* | Yes  | Yes  |
| EVE_CMD_TESTCARD         | CMD_TESTCARD         | *No* | *No* | *No* | Yes  | Yes  |
| EVE_CMD_WAIT             | CMD_WAIT             | *No* | *No* | *No* | Yes  | Yes  |
| EVE_CMD_NEWLIST          | CMD_NEWLIST          | *No* | *No* | *No* | Yes  | Yes  |
| EVE_CMD_ENDLIST          | CMD_ENDLIST          | *No* | *No* | *No* | Yes  | Yes  |
| EVE_CMD_CALLLIST         | CMD_CALLLIST         | *No* | *No* | *No* | Yes  | Yes  |
| EVE_CMD_RETURN           | CMD_RETURN           | *No* | *No* | *No* | Yes  | Yes  |
| EVE_CMD_NOP              | CMD_NOP              | *No* | *No* | Yes  | Yes  | Yes  |
| EVE_CMD_FILLWIDTH        | CMD_FILLWIDTH        | *No* | *No* | Yes  | Yes  | Yes  |
| EVE_CMD_ROTATEAROUND     | CMD_ROTATEAROUND     | *No* | *No* | Yes  | Yes  | Yes  |
| EVE_CMD_RESETFONTS       | CMD_RESETFONTS       | *No* | *No* | Yes  | Yes  | Yes  |
| EVE_CMD_GRADIENTA        | CMD_GRADIENTA        | *No* | *No* | Yes  | Yes  | Yes  |
| EVE_CMD_FLASHERASE       | CMD_FLASHERASE       | *No* | *No* | Yes  | Yes  | Yes  |
| EVE_CMD_FLASHWRITEEXT    | CMD_FLASHWRITEEXT    | *No* | *No* | Yes  | Yes  | Yes  |
| EVE_CMD_FLASHWRITE       | CMD_FLASHWRITE       | *No* | *No* | Yes  | Yes  | Yes  |
| EVE_CMD_FLASHUPDATE      | CMD_FLASHUPDATE      | *No* | *No* | Yes  | Yes  | Yes  |
| EVE_CMD_FLASHREAD        | CMD_FLASHREAD        | *No* | *No* | Yes  | Yes  | Yes  |
| EVE_CMD_FLASHPROGRAM     | CMD_FLASHPROGRAM     | *No* | *No* | Yes  | Yes  | Yes  |
| EVE_CMD_FLASHSOURCE      | CMD_FLASHSOURCE      | *No* | *No* | Yes  | Yes  | Yes  |
| EVE_CMD_FLASHSPITX       | CMD_FLASHSPITX       | *No* | *No* | Yes  | Yes  | Yes  |
| EVE_CMD_FLASHFAST        | CMD_FLASHFAST        | *No* | *No* | Yes  | Yes  | Yes  |
| EVE_CMD_FLASHSPIRX       | CMD_FLASHSPIRX       | *No* | *No* | Yes  | Yes  | Yes  |
| EVE_CMD_FLASHATTACH      | CMD_FLASHATTACH      | *No* | *No* | Yes  | Yes  | Yes  |
| EVE_CMD_FLASHDETATCH     | CMD_FLASHDETATCH     | *No* | *No* | Yes  | Yes  | Yes  |
| EVE_CMD_FLASHSPIDESEL    | CMD_FLASHSPIDESEL    | *No* | *No* | Yes  | Yes  | Yes  |
| EVE_CMD_GETIMAGE         | CMD_GETIMAGE         | *No* | *No* | *No* | Yes  | Yes  |
| EVE_CMD_CALIBRATESUB     | CMD_CALIBRATESUB     | *No* | *No* | *No* | Yes  | Yes  |
| EVE_CMD_COPYLIST         | CMD_COPYLIST         | *No* | *No* | *No* | *No* | Yes  |
| EVE_CMD_CGRADIENT        | CMD_CGRADIENT        | *No* | *No* | *No* | *No* | Yes  |
| EVE_CMD_TEXTDIM          | CMD_TEXTDIM          | *No* | *No* | *No* | *No* | Yes  |
| EVE_CMD_ARC              | CMD_ARC              | *No* | *No* | *No* | *No* | Yes  |
| EVE_CMD_RENDERTARGET     | CMD_RENDERTARGET     | *No* | *No* | *No* | *No* | Yes  |
| EVE_CMD_ENABLEREGION     | CMD_ENABLEREGION     | *No* | *No* | *No* | *No* | Yes  |
| EVE_CMD_FENCE            | CMD_FENCE            | *No* | *No* | *No* | *No* | Yes  |
| EVE_CMD_GRAPHICSFINISH   | CMD_GRAPHICSFINISH   | *No* | *No* | *No* | *No* | Yes  |
| EVE_CMD_REGWRITE         | CMD_REGWRITE         | *No* | *No* | *No* | *No* | Yes  |
| EVE_CMD_APBWRITE         | CMD_APBWRITE         | *No* | *No* | *No* | *No* | Yes  |
| EVE_CMD_APBREAD          | CMD_APBREAD          | *No* | *No* | *No* | *No* | Yes  |
| EVE_CMD_LOADWAV          | CMD_LOADWAV          | *No* | *No* | *No* | *No* | Yes  |
| EVE_CMD_LOADASSET        | CMD_LOADASSET        | *No* | *No* | *No* | *No* | Yes  |
| EVE_CMD_LOADPATCH        | CMD_LOADPATCH        | *No* | *No* | *No* | *No* | Yes  |
| EVE_CMD_GLOW             | CMD_GLOW             | *No* | *No* | *No* | *No* | Yes  |
| EVE_CMD_SDATTACH         | CMD_SDATTACH         | *No* | *No* | *No* | *No* | Yes  |
| EVE_CMD_FSOPTIONS        | CMD_FSOPTIONS        | *No* | *No* | *No* | *No* | Yes  |
| EVE_CMD_FSREAD           | CMD_FSREAD           | *No* | *No* | *No* | *No* | Yes  |
| EVE_CMD_FSSIZE           | CMD_FSSIZE           | *No* | *No* | *No* | *No* | Yes  |
| EVE_CMD_FSSOURCE         | CMD_FSSOURCE         | *No* | *No* | *No* | *No* | Yes  |
| EVE_CMD_FSDIR            | CMD_FSDIR            | *No* | *No* | *No* | *No* | Yes  |
| EVE_CMD_SDBLOCKREAD      | CMD_SDBLOCKREAD      | *No* | *No* | *No* | *No* | Yes  |
| EVE_CMD_WAITCHANGE       | CMD_WAITCHANGE       | *No* | *No* | *No* | *No* | Yes  |
| EVE_CMD_WAITCOND         | CMD_WAITCOND         | *No* | *No* | *No* | *No* | Yes  |
| EVE_CMD_RESULT           | CMD_RESULT           | *No* | *No* | *No* | *No* | Yes  |
| EVE_CMD_I2SSTARTUP       | CMD_I2SSTARTUP       | *No* | *No* | *No* | *No* | Yes  |

### Creating screens and executing commands

The API Layer provides functions to begin and end lists of co-processor commands. The co-processor commands must be preceded and followed by co-processor management functions.

#### Writing DL Instructions and Co-Processor Commands

Using EVE commands via the co-processor requires some data formatting to convert the parameters of the command into the correct hex values to be sent as well as keeping track of the number of bytes sent to update the write pointer correctly. Some commands also require padding to make their total size including parameters a multiple of 4 bytes. The functions in EVE_API hide this from the main application.

#### Beginning and Ending Co-Processor Lists

All co-processor lists must begin with a call to `EVE_LIB_BeginCoProList`.  
If any display list items or co-processor commands which use the display list are to be added then a call to `EVE_CMD_DLSTART` is required immediately after this.

For the avoidance of doubt, commands that only read or write registers, read or write memory, access flash or access the SD card do not require the `EVE_CMD_DLSTART` call.

All co-processor lists displaying graphics would be preceded by:

```c
    EVE_LIB_BeginCoProList(); // CS low and send address in RAM_CMD 
    EVE_CMD_DLSTART(); // When executed, EVE will begin a new DL
```

And followed by:

```c
    EVE_LIB_EndCoProList(); // CS high
    EVE_LIB_AwaitCoProEmpty(); // Wait for FIFO to be finish
```

A call to `EVE_LIB_AwaitCoProEmpty` is implied in the call to `EVE_LIB_BeginCoProList`. Therefore it is not necessary to wait at the end of the co-processor  
list for the completion of the commands allowing program to perform other tasks not related to programming the EVE device.

The `EVE_LIB_AwaitCoProEmpty` function will return zero if the co-processor commands have run successfully. If there was an error with a co-processor command or data used by the co-processor then an exception can be raised which will require the application to handle. The Programming Guide for each generation details the actions required when this occurs. See the section called "Coprocessor Faults" or "Fault Scenarios".

On EVE API 3, 4 and 5 there is a text message generated by the co-processor with a brief description of the fault. This message can be obtained with the `EVE_LIB_GetCoProException` function.

#### Simple Co-Processor List

The following is a simple list to write text on the screen in white letters:

```c
    EVE_LIB_BeginCoProList(); // CS low and send address in RAM_CMD 
    EVE_CMD_DLSTART(); // When executed, EVE will begin a new DL
    
    EVE_CLEAR_COLOR_RGB(0, 0, 0); // Select colour to clear screen to 
    EVE_CLEAR(1,1,1); // Clear screen

    EVE_COLOR_RGB(255, 255, 255);
    EVE_CMD_TEXT(100, 100, 28, EVE_OPT_CENTERX | EVE_OPT_CENTERY, "Hello");
    
    EVE_DISPLAY(); // Tells EVE that this is the end 
    EVE_CMD_SWAP(); // Swaps new list into foreground buffer  
    EVE_LIB_EndCoProList(); // CS high and end list 
    EVE_LIB_AwaitCoProEmpty(); // Wait for FIFO to be empty 
    // (commands executed) 
```

To send a display list to the screen the commands `EVE_CMD_DLSTART` is required at the beginning of a co-processor list before any display list items are added.

To finish the `EVE_DISPLAY` command is sent to the display list then the `EVE_CMD_SWAP` co-processor command is used to effect the change of display list being rendered on the screen.

#### Executing a Single Co-Processor Command

When just executing a co-processor command (for example calling CMD_SETROTATE during set-up of the application to set the screen orientation) then the following can be used:

```c
    EVE_LIB_BeginCoProList(); // CS low and send address in RAM_CMD 

    EVE_CMD_SETROTATE(2);

    EVE_LIB_EndCoProList(); // CS high
    EVE_LIB_AwaitCoProEmpty(); // Wait for FIFO to be finish
```

If there is no display list created for a set of co-processor commands then there is no need for the `EVE_CMD_DLSTART`, `EVE_DISPLAY` or `EVE_CMD_SWAP`.

#### Large Co-Processor Lists

On EVE1, EVE2, EVE3 and EVE4 there is 4 kB of co-processor list buffer space, on EVE5 there is 16 kB. A large co-processor list can use the whole buffer space many times over.  
If an image is being decoded with `EVE_CMD_LOADIMAGE` then wrapping around the buffer space is a common occurrance.

The simpler examples above are small and do not need to check how much space is remaining in the co-processor list buffer. Large lists that potentially wrap the buffer space need a better stategy.

Register reads are not allowed within a co-processor list. However, a co-processor list can be created in more than one section as shown below. For tasks sending long lists, the data can be divided into smaller chunks and sent with the program waiting for sufficient buffer space before continuing with the next chunk.

The `EVE_LIB_WriteDataToCMD` implements an efficient strategy to do this.

This example below shows how to split a co-processor list to generate a display list correctly. Note the position of the `EVE_CMD_DLSTART`, `EVE_DISPLAY` or `EVE_CMD_SWAP` commands.

```c
  // FIRST SECTION OF LIST
  EVE_LIB_BeginCoProList(); // CS low and send address in RAM_CMD
  EVE_CMD_DLSTART(); // When executed, EVE will begin a new DL
  EVE_CLEAR_COLOR_RGB(0, 0, 0); // Select colour to clear screen 
  EVE_CLEAR(1,1,1); // Clear the screen
  EVE_COLOR_RGB(255, 255, 255);
  EVE_LIB_EndCoProList(); // CS high 
  EVE_LIB_AwaitCoProEmpty(); // Wait for FIFO to be empty 
  // (commands executed)
  // **** You can write or read registers here ****
  // SECOND SECTION OF LIST
  EVE_LIB_BeginCoProList(); // CS low and send address in RAM_CMD
  EVE_CMD_TEXT(100, 100, 28, OPT_CENTERX|OPT_CENTERY,"Hello");
  EVE_DISPLAY(); // Tells EVE that this is the end 
  EVE_CMD_SWAP(); // Swaps new list into foreground buffer  
  EVE_LIB_EndCoProList(); // CS high
  EVE_LIB_AwaitCoProEmpty(); // Wait for FIFO to be empty 
  // (commands executed) 
```

The above sequence will create the same set of commands in RAM_DL as the code below.

```c
  EVE_LIB_BeginCoProList(); // CS low and send address in RAM_CMD
  EVE_CMD_DLSTART(); // When executed, EVE will begin a new DL 
  EVE_CLEAR_COLOR_RGB(0, 0, 0); // Select colour to clear screen to
  EVE_CLEAR(1,1,1); // Clear the screen
  EVE_COLOR_RGB(255, 255, 255);
  EVE_CMD_TEXT(100, 100, 28, EVE_OPT_CENTERX | EVE_OPT_CENTERY, "Hello");  
  EVE_DISPLAY(); // Tells EVE that this is the end  
  EVE_CMD_SWAP(); // Swaps new list into foreground buffer  
  EVE_LIB_EndCoProList(); // CS high 
  EVE_LIB_AwaitCoProEmpty(); // Wait for FIFO to be empty 
  // (commands executed)
```

The usage is fundamentally the same as the library and examples described in BRT_AN_008 (FT81x Creating a Simple Library For PIC MCU) and BRT_AN_014 (FT81X Simple PIC Library Examples) and so these can be used as a reference when using this library.

The API function `EVE_LIB_GetCoProSpace` can be used to check if there is sufficient space available in the co-processor for further commands to be sent. The command will not stop and restart the co-processor lists as in the example above but will pause the co-processor list to perform a register read before resuming another transfer without interrupting the program flow.

#### Profiling the Co-processor List

Setting the `EVE_COPROC_PROFILE` macro will enable code that can count the number of bytes sent to the co-processor. This is useful to find out the size of each co-processor list.

It is initilised using `EVE_LIB_BeginCoProProfile` at the beginning of a list to measure. The call to `EVE_LIB_GetCoProProfile` will return the number of bytes written since the list profiling was initialised.

This feature can be used in conjunction with `EVE_LIB_GetCoProSpace` to predict the size of the co-processor fullness.

Enabling the macro will add one 16-bit storage variable to the compiled project.

#### Limitations in RAM_DL and RAM_CMD

It is important to note that the overall limit of 8K for the generated RAM_DL list still applies, even if lists are sent in multiple sections. It is also important to bear in mind that the size of a co-processor command is not always the same as the size of the resulting RAM_DL instructions which the co-processor generates from the commands.

For example, the CMD_BUTTON uses 16 bytes of RAM_CMD plus the size of the string (plus any string arguments in BT81x) for the command, but the graphic operations in RAM_DL which the co-processor creates to render the button will be larger than this. The 8K RAM_DL limit does not therefore mean that 8K of co-processor commands can be used in one list.

REG_CMD_DL indicates the next available location in RAM_DL and so after executing a list commands (but before the swap) this register can be used to check how full RAM_DL is. The value read will be between 0 and 8191 with 8191 indicating the RAM_DL is full.

The value of REG_CMD_DL is read after executing the commands above but before the swap is executed. The swap is sent using a separate transaction (beginning with `EVE_LIB_BeginCoProList` and ending with `EVE_LIB_EndCoProList` and `EVE_LIB_AwaitCoProEmpty` ) because a register read or write cannot take place whilst an existing SPI transaction (burst write or read) is in progress.  Note that in this example the `EVE_LIB_Read16` is used and will work on EVE APIs 1 to 4, on EVE 5 only 32-bit reads and writes are supported.

```c
  EVE_LIB_BeginCoProList(); // CS low and send address in RAM_CMD
  EVE_CMD_DLSTART(); // When executed, EVE will begin a new DL 
  EVE_CLEAR_COLOR_RGB(0, 0, 0); // Select color to clear screen to  
  EVE_CLEAR(1,1,1); // Clear the screen
  EVE_COLOR_RGB(255, 255, 255);
  EVE_CMD_TEXT(100, 100, 28, EVE_OPT_CENTERX | EVE_OPT_CENTERY, "Hello");  
  EVE_DISPLAY(); // Tells EVE that this is end of the list  
  EVE_LIB_EndCoProList(); // CS high 
  EVE_LIB_AwaitCoProEmpty(); // Wait for FIFO to be empty 
  // (commands executed)
  uint16_t RAM_DL_fullness = EVE_LIB_Read16(EVE_REG_CMD_DL); // check value in MCU debugger or print to UART etc.
  EVE_LIB_BeginCoProList(); // CS low and send address in RAM_CMD 
  EVE_CMD_SWAP(); // Swaps new list into foreground buffer  
  EVE_LIB_EndCoProList(); // CS high 
  EVE_LIB_AwaitCoProEmpty(); // Wait for FIFO to be empty 
  // (commands executed)
```

#### Writing RAM_G and RAM_CMD

These functions allow burst writes to RAM_G and RAM_CMD. Individual bursts must not exceed 65535 bytes; larger transfers must be split into smaller sections. The HAL further divides transfers into chunks of up to `EVE_HAL_CHUNK_SIZE` bytes, while the underlying MCU or Platform implementation may apply additional host-interface-specific transfer limits.

```c
void EVE_LIB_WriteDataToRAMG(const uint8_t *ImgData, uint32_t DataSize, uint32_t DestAddress)
```

This function performs an SPI burst write to RAM_G. The starting address, as well as the source of the data and amount of data are specified. EVE can be written in a similar fashion to an SPI memory device. After asserting CS and sending the address, data can be written as a burst whilst keeping CS low. A similar function performs a read of the selected memory.

```c
void EVE_LIB_WriteDataToCMD(const uint8_t *ImgData, uint32_t DataSize) 
```

This function allows a block of data to be written to RAM_CMD which is needed when writing data to be inflated for example. This is more complex as the circular nature of the buffer must be handled in addition to splitting data into chunks since the buffer is only 4K in size. This function handles the entire process and so makes writing to RAM_CMD as simple as to RAM_G for the layers above. A flow chart can be found in BRT_AN_008 (FT81x Creating a Simple Library For PIC MCU) for loading data via the co-processor buffer RAM_CMD.  
Other helper functions are provided such as for writing strings and for retrieving co-processor results (as some commands such as CMD_GETPROPS return their result via RAM_CMD).

```c
uint16_t EVE_LIB_SendString(const char* string)
```

This function sends a string of characters and is used by commands such as CMD_TEXT, CMD_BUTTON and CMD_TOGGLE which all use text strings. This function takes care of the extra padding which is required as all EVE commands must be 32-bit aligned. Therefore, depending on the length of the string (plus the necessary null character to terminate it) then between one and three extra "\0" (NUL) bytes are added to pad the command to be a multiple of 4 bytes. The main application can therefore send strings without needing to consider the padding.

#### Handling Interrupts

The interrupt register `REG_INT_FLAGS` is provided to allow an application to see if one of several interrupt events are flagged. These can be polled by reading the register. However, the register is automatically cleared on each read.

The API provides a method for accessing the register and preserving any tested flags for later testing. The `EVE_LIB_GetInterrupt` function is provided to load and store the current bits set to keep a set of flags set in a global variable. There is a mask value as a parameter to the function which is used to test the set bits. Once the bits have been tested in the global variable they can be cleared.

For example, if a key press was detected and the bit set in the register during the period the application was waiting for a command buffer empty event the API can be queried with `EVE_LIB_GetInterrupt(EVE_INT_CMD_EMPTY)`. The `EVE_INT_TOUCH` bit would be unaffected and the application could later independently test the command for that event.

Enabling the macro will add one 8-bit storage variable to the compiled project.

#### Accessing the INT# line

The optional INT# line is provided for the EVE device to signal to the host MCU that an event has occurred.

This status can be accessed from the EVE API with the `EVE_LIB_Int` function. A value greater than zero indicates that the INT# line is asserted. On MCUs and Platforms that do not support reading the INT# line the return value will be -1.

## Programming Guides

The Programming Guides for each generation of EVE device is found in the [Bridgetek Programming and User Guides](https://brtchip.com/document/programming-guides/).
