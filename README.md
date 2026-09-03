# EVE-MCU-Dev

This library allows a variety of hardware to communicate with FT8xx and BT8xx graphics controller devices: embedded MCUs using their native SPI hardware; Linux PCs using SPI character devices; PCs using FT4222H or MPSSE USB devices. 

This library is intended to provide a **C** library for embedded designs.


## Contents

- [Overview](#overview)
  - [History](#history)
  - [Scope](#scope)
  - [Prerequisites](#prerequisites)
- [Software Layers](#software-layers)
  - [Folder Structure](#folder-structure)
    - [Common Library Files](#common-library-files)
    - [Port Files](#port-files)
    - [Example Files](#example-files)
  - [Device and Panel Selection](#device-and-panel-selection)
    - [Device Selection](#device-selection)
    - [Display Panel Selection](#display-panel-selection)
- [Ports](#ports)
- [Examples Code](#example-code)
- [Module Connections](#module-connections)
  - [Through-Board 2x8 Pins](#through-board-2x8-pins)
  - [Header 1x10 Pins](#header-1x10-pins)
- [Library Usage](#library-usage)
  - [Initialising EVE](#initialising-eve)
  - [Co-Processor Helpers](#co-processor-helpers)
  - [Creating screens and executing commands](#creating-screens-and-executing-commands)
    - [Writing DL Instructions and Co-Processor Commands](#writing-dl-instructions-and-co-processor-commands)
    - [Beginning and Ending Co-Processor Lists](#beginning-and-ending-co-processor-lists)
    - [Simple Co-Processor List](#simple-co-processor-list)
    - [Executing a Single Co-Processor Command](#executing-a-single-co-processor-command)
    - [Co-Processor Lists of more than 4K Size](#co-processor-helpers)
    - [Limitations in RAM_DL and RAM_CMD](#limitations-in-ram_dl-and-ram_cmd)
    - [Writing RAM_G and RAM_CMD](#writing-ram_g-and-ram_cmd)

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

The library structure is designed to provide a format where multiple examples with support for multiple platforms can be presented.

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
- `EVE_HAL.c` The abstraction layer between the programming interface and the MCU specific layer.
- `EVE_HAL_Linux.c` The abstraction layer between the programming interface and the Linux SPI character device.

Include files show the layers inherent in the library. The main API can be accessed with just the `EVE.h` header file which will include all the required header files to compile using the EVE API. The configuration for the display panel and other relvant configurable settings is modified in the `EVE_config.h` file. For most applications this is the only file in the library that will need modification.

Contents of the `include` directory:

- **`EVE.h`** Header file to include to access all required programming interface entry points and definitions.
- **`EVE_config.h`** Overridable configuration file for target application.
- `EVE_commands.h` Header file which provides cross-generation EVE command and option definitions.
- `EVE_debug.h` Header file which provides platform specific macro definitions for debug messaging.
- `EVE_registers.h` Header file which provides cross-generation EVE register address map.
- `HAL.h` Definitions for accessing the abstraction layer from the API layer.
- `MCU.h` Embedded header file for access to the MCU layer from the abstraction layer.
- `Platform.h` _Linux-like_ header file for access to the MCU layer from the abstraction layer.

**Bold** files are the files with the recommended access points for a program into the library.

Extension-specific functionality is separated from the common EVE API source and header files. Extension header files are located in `include/extensions`, with their corresponding implementations located in `source/extensions`. These files provide functionality which is required only for specific EVE device generations or configurations and can be excluded from projects if the are not required.

* `/source/extensions/bt82x_patch.c` Implementation of the BT82x base patch loader and the additional API commands provided by the base patch for EVE API level 5 devices.
* `/include/extensions/bt82x_patch.h` Definitions and function declarations for the BT82x base patch functionality.
* `/source/extensions/custom_touch_fw.c` Implementation for loading custom touch firmware into supported EVE devices when the `EVE_CUSTOM_TOUCH` define is enabled.
* `/include/extensions/custom_touch_fw.h` Function declarations for the custom touch firmware extension.

The extension source files are included in the build only where required for the selected EVE API or configuration. Extension headers are referenced through the main `include` directory, for example `#include <extensions/bt82x_patch.h>`.

#### Port Files

The ports directory has folder for each platform supported. These will contain a file that implements the interface described in `MCU.h` or `Platform.h` files in the `include` directory. This will deal with any byte-order changing required and all access to the GPIO and SPI interfaces. 

It is further discussed in the [Ports](#ports) section.

#### Example Files

The examples directory contains all the examples provided. There are more details in the [Example Code](#example-code) section.

### Device and Panel Selection

The library __must__ be compiled for the correct EVE device and panel type. The target EVE device and panel type are defined in the file `EVE_config.h`.

It is **recommended** that the `EVE_config.h` file is modified in a user program by including the modified version before the library version in the search path for include files passed to the compiler.

There are three methods of configuring the EVE device and panel type. 
- The `EVE_DEVICE` macro and `EVE_DISPLAY_RES` macro. (Formerly the `FT8XX_TYPE` macro and `DISPLAY_RES` macro)
  This is the simplest method if a configuration is fixed. The `EVE_MODULE` and `EVE_PANEL` macros may be removed or be set to `EVE_NO_MODULE` and `PANEL_TYPE_NONE` respectively.
- The `EVE_DEVICE` macro and `EVE_PANEL` macro.
  This sets the `EVE_DISPLAY_RES` for a panel. The `EVE_MODULE` macros may be removed or be set to `EVE_NO_MODULE`.
- A Bridgetek module type may be set. 
  This will configure the `EVE_DEVICE` and `EVE_PANEL` macros. 
  The `EVE_PANEL` macro will be further expanded into a `EVE_DISPLAY_RES` macro.
  
In all cases the `EVE_DISPLAY_RES` macro will lead to the `EVE_DISP_*` macros being set for configuring the registers on in on the EVE device. 

The `EVE_DISPLAY_RES` macro is not used in the library.

The `EVE_PANEL` macro is not used in the library, however it is optionally used in the `examples/snippets/touch.c` examples snippet code to set predefined touchscreen configuration values to bypass calibration.

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
  | **EVE_RES_QVGA**    | 320 x 240   | [DP-0351-11A](https://brtchip.com/product/dp-0351-11a/) | 
  | **EVE_RES_WQVGA**   | 320 x 240   | [DP-0431-11A](https://brtchip.com/product/dp-0431-11a/), [DP-0502-11A](https://brtchip.com/product/dp-0502-11a/) |
  | **EVE_RES_WQVGAR**  | 480 x 480   | [IDM2040-21R](https://brtchip.com/product/idm2040-21r/) with 2.1 inch round display |
  | **EVE_RES_WVGA**    | 800 x 480   | [DP-0501-01A](https://brtchip.com/product/dp-0501-01a/), [DP-0501-11A](https://brtchip.com/product/dp-0501-11a/), [DP-0701-11A](https://brtchip.com/product/dp-0701-01a/) |
  | **EVE_RES_WSVGA**   | 1024 x 600  | [ME817EV](https://brtchip.com/product/me817ev/) with 7 inch display |
  | **EVE_RES_WXGA**    | 1280 x 800  | [DP-1011-01A](https://brtchip.com/product/dp-1011-01a/) |
  | **EVE_RES_WXGA_NG** | 1280 x 800  | [DP-1011-02A](https://brtchip.com/product/dp-1011-02a/) |
  | **EVE_RES_FULLHD**  | 1920 x 1080 | [DP-1561-01A](https://brtchip.com/product/dp-1561-01a/), [DP-1561-02A](https://brtchip.com/product/dp-1561-02a/) |
  | **EVE_RES_WUXGA**   | 1920 x 1200 | [DP-1012-01A](https://brtchip.com/product/dp-1012-01a/) |
  

- `EVE_PANEL` The Bridgetek panel type of the display panel.
  The following panels are defined:
  | Panel Name | Description | Touch Type |
  | ----- | ----- | ----- |
  | **EVE_DP_0351_11A** | 3.5 inch display panel (**QVGA**) |  Resistive | 
  | **EVE_DP_0431_11A** | 4.3 inch display panel (**WQVGA**) | Resistive  |
  | **EVE_DP_0501_01A** | 5 inch display panel (**WVGA**) | Capacitive  |
  | **EVE_DP_0501_11A** | 5 inch display panel (**WVGA**) | Resistive |
  | **EVE_DP_0502_11A** | 5 inch display panel (**WQVGA**) |  Resistive |
  | **EVE_DP_0701_01A** | 7 inch display panel (**WVGA**) | Capacitive |
  | **EVE_DP_1011_01A** | 10.1 inch display panel (**WXGA**) | Capacitive |
  | **EVE_DP_1011_02A** | 10.1 inch display panel (**WXGA_NG**) | Capacitive |
  | **EVE_DP_1012_01A** | 10.1 inch  display panel (**WUXGA**) |  Capacitive |
  | **EVE_DP_1561_01A** | 15.6 inch display panel (**FullHD**) | Capacitive |
  | **EVE_DP_1561_02A** | 15.6 inch display panel (**FullHD**) | Capacitive |
  | **EVE_DP_IDM43A**   | 4.3 inch display panel (**WQVGA**) | Capacitive |
  | **EVE_DP_IDM21R**   | 2.1 inch display panel (**WQVGAR**) | Capacitive |

- `EVE_MODULE` The Bridgetek module or development kit type for EVE device and display panel. The following options are defined:
  | Module or Kit Name | Description |
  | ----- | ----- |
  | **EVE_VM800B**      | [VM800B35A-BK](https://brtchip.com/product/vm800b35a-bk/) with 3.5 inch display. (**FT800** with **DP-0351-11A**) |
  | **EVE_VM800C35A**   | [VM800C35A-D](https://brtchip.com/product/vm800c35a-d/) with 3.5 inch display. (**FT800** with **DP-0351-11A**) |
  | **EVE_VM800C43A**   | [VM800C43A-D](https://brtchip.com/product/vm800c43a-d/) with 4.3 inch display. (**FT800** with **DP-0431-11A**) |
  | **EVE_VM800C50A**   | [VM800C50A-D]() with 5 inch display. (**FT800** with **DP-0502-11A**) |
  | **EVE_VM810C**      | [VM810C50A-D](https://brtchip.com/product/vm810c50a-d/) with 5 inch display. (**FT810** with **DP-0501-11A**) |
  | **EVE_ME812A**      | [ME812A-WH50R](https://brtchip.com/product/me812a-wh50r/), [ME812AU-WH50R](https://brtchip.com/product/me812au-wh50r/) with 5 inch display. (**FT812** with **DP-0501-11A**) |
  | **EVE_ME813A**      | [ME813A-WH50C](https://brtchip.com/product/me813a-wh50c/) with 5 inch display. (**FT813** with **DP-0501-01A**) |
  | **EVE_VM816C**      | [VM816C50A-D](https://brtchip.com/product/vm816c50a-d/), [VM816CU50A-D](https://brtchip.com/product/vm816cu50a-d/) with 5 inch display. (**BT816** with **DP-0501-11A**) |
  | **EVE_VM880C**      | [VM880C](https://brtchip.com/product/vm880c/) with assumed 4.3 inch display. (**BT880** with **DP-0431-11A**) |
  | **EVE_IDM204021R**  | [IDM2040-21R](https://brtchip.com/product/idm2040-21r/) (**FT800** with 2.1 inch display) |
  | **EVE_IDM204043A**  | [IDM2040-43A](https://brtchip.com/product/idm2040-43a/) (**BT883** with **DP-0431-11A**) |
  | **EVE_IDM20407A**   | [IDM2040-7A](https://brtchip.com/product/idm2040-7a/) (**BT817** with **DP-0701-01A**) |
  | **EVE_VM820B10A**   | [VM820B10A](https://brtchip.com/product/vm820b10a/) with 10.1 inch display. (**BT820** with **DP-1011-02A**) |
  | **EVE_VM820B15A**   | [VM820B15A](https://brtchip.com/product/vm820b15a/) with 15.6 inch display. (**BT820** with **DP-1561-02A**) |
  | **EVE_IDK_FT810_43A**   | [FT810 IC Development Kit](https://brtchip.com/product/idk-ft810-43a/) with 4.3-inch display. (**FT810** with **DP-0431-11A**) |
  | **EVE_IDK_BT816_50A**   | [BT816 IC Development Kit](https://brtchip.com/product/idk-bt816-50a/) with 5.0-inch display. (**BT816** with **DP-0501-11A**) |
  | **EVE_IDK_BT817_70A**   | [BT817 IC Development Kit](https://brtchip.com/product/idk-bt817-70a/) with 7.0-inch display. (**BT817** with **DP-0701-01A**) |
  | **EVE_IDK_BT817_101A**   | [BT817 IC Development Kit](https://brtchip.com/product/idk-bt817-101a/) with 10.1-inch display. (**BT817** with **DP-1011-02A**) |
  | **EVE_IDK_BT820_101A**   | [BT820 IC Development Kit](https://brtchip.com/product/idk-bt820-101a/) with 10.1-inch display. (**BT820** with **DP-1012-01A**) | 

#### Device Selection

The EVE device to target is set in the file `EVE_config.h`. The macro `EVE_DEVICE` or `EVE_API`/`EVE_SUB_API` is set to choose the device or the API respectively. One or other of these macros **must** be set correctly for the device being used.

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
**The default in the distribution will be a BT817 device**. This is the EVE device used in the IDK-BT817-70A modules.

Note that the example programs will take the `EVE_config.h` file from the `include` directory.

#### Display Panel Selection

The display panel dimensions to use are set in the file `EVE_config.h` using the `EVE_DISPLAY_RES` or `EVE_PANEL` macros.

The macro `EVE_DISPLAY_RES` will enable one of the pre-defined panel settings to be configured with the register values needed for that panel type. The registers are calculated for the standard Bridgetek panels in the resolution indicated by the `EVE_DISPLAY_RES` macro. Other panels may require different register settings. If a new panel is needed then the settings can be derived from the panel specifications or contact Bridgetek Support for advice.

The display panel settings **must** be correct for the panel in used otherwise it is unlikely that there will be any output visible.

**The default in the distribution will be a WVGA panel**. This is the panel used in the IDK-BT817-70A modules.

#### Setting Device and Panel in Build Configuration

The `EVE_MODULE`, `EVE_DEVICE`, `EVE_PANEL` and `EVE_DISPLAY_RES` macros can be set in a build file as a C define. This can be used to change the configuration without editing or changing the `EVE_config.h` file. 

The `EVE_MODULE` macro is parsed first. Setting this to `EVE_NO_MODULE` will allow one or all of the `EVE_DEVICE`, `EVE_PANEL` and `EVE_DISPLAY_RES` macros to be picked up from the build file C definitions. 

Note that the preprocessor may complain if it is asked to change the value of one of the macros. 

## Ports

The supported platforms are listed in the [ports/README.md](ports/README.md) file. 

The source code for each platform is stored in the [ports](ports) directory. Each source file in each ports folder is guarded by one of the PLATFORM_<i>xxx</i> macros, USE_<i>xxx</i> macros, or a development environment specific macro. This way all the files in the ports directory can be loaded into a compiler and ignored if they are not relevant.

## Example Code

There are example projects for many each supported platform. The [examples/README.md](examples/README.md) file has details on each of the included examples.

The ["simple"](examples/simple/README.md) example has build environments for all platforms and forms the basis of other examples that are provided. Build instructions are included in the ["simple" example directory.](examples/simple/README.md).

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

Before using the library to send instructions to the EVE device the `EVE_Init()` function **must** be called.

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

### Co-Processor Helpers

These functions perform the necessary tasks to begin and execute co-processor lists.

#### EVE_LIB_BeginCoProList

Begin co-processor list.

**Detailed description:**

Starts a co-processor list. Initialises the API and HAL ready to start
transmitting a co-processor list to the EVE.
This will typically assert chip select to allow the SPI interface to
send data to the EVE.

**Format:**

`void EVE_LIB_BeginCoProList(void)`

#### EVE_LIB_EndCoProList

End co-processor list.
 
**Detailed Description:** 

Ends a co-processor list. This will perform any operations in the API
and HAL to finish a co-processor list. 
This will typically deasserts chip select after updating any registers
on the EVE device that will signal the end of the co-processor list.

**Format:**

`void EVE_LIB_EndCoProList(void)`

#### EVE_LIB_AwaitCoProEmpty

Waits for co-processor list to end.

**Detailed Description:**

Will poll the co-processor command list until it has been completed.
If configured then it will wait for an interrupt signal on the INT# line before testing for correct completion.

**Returns:** 

- 0 for successful completion.
- 0xff for co-processor exception.

**Format:**

`int EVE_LIB_AwaitCoProEmpty(void)`

Waits for the completion of the current commands sent to the co-processor.

#### EVE_LIB_AwaitCoProEmptyTimeout

Waits for co-processor list to end with a millisecond timeout.

**Detailed Description:**

Will poll the co-processor command list until it has been completed or a timeout has occurred.
If configured then it will wait for an interrupt signal on the INT# line before testing for correct completion.

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

**Detailed Description:**

Will reset the co-processor after an exception is reported by EVE_LIB_AwaitCoProEmpty.

**Format:** 

`void EVE_LIB_RecoverCoPro(void)`

#### EVE_LIB_GetCoProSpace

Returns the sapce remaining for further commands to be sent to the co-processor.

**Detailed Description:**

Obtains the free space in the co-processor circular buffer. 
This operation may have an effect on the performance of the device.

**Returns:**

The number of free bytes in the co-processor circular buffer.

**Format:** 

`uint16_t EVE_LIB_GetCoProSpace(void)`

#### EVE_LIB_Int

Test interrupt input line.

 **Detailed Description:**
 
 This function will check the interrupt input INT# from
 the EVE device. If Quad SPI is enabled then the interrupt line
 is used as a data line for SPI and therefore cannot be used for
 an interrupt input.
 
**Returns:**

- zero if there is no interrupt.
- >0 if the EVE device is asserting an interrupt.
- -1 if the MCU or Platform does not support reading the interrupt line.

**Format:** 

`int EVE_LIB_Int(void)`

#### EVE_LIB_GetInterrupt

Test if an interrupt flag is set.
 
**NOTE:** This is only compiled if the co-processor method is set to `EVE_COPROC_INT`.

**Detailed Description:**

Will read the interrupt flag register and add any newly pending to
a status value. The flag register will clear any pending interrupt
when read so the cumulative flagged bits are kept until they are
cleared by the mask in this function.

**Returns:**

- 0 for no interrupts in the mask being set.
- if any interrupts are set then the return value will contain bits set from the mask parameter.

**Format:** 

`uint8_t EVE_LIB_GetInterrupt(uint8_t mask)`

**Parameters:**

- *mask* - bit mask of interrupts to test. Unmasked interrupts are not modified.

#### EVE_LIB_GetResult

Returns a result from the co-processor command buffer.

**Detailed Description:**

Will return a result value from "offset" words back in the command buffer.
If the value of offset is 1 then the previous value from the co-processor
command buffer is returned.

**Returns:**

Result of a previous co-processor command.

**Format:** 

`uint32_t EVE_LIB_GetResult(int offset)`

**Parameters:**

- *offset* - number of 32-bit words to go back in the command buffer for the result.

#### EVE_LIB_GetCoProException

Get co-processor exception description.

**NOTE:** This is only available on EVE APIs 3, 4 and 5.

**Detailed Description:**

Will query the co-processor exception description to a string.

**Returns:**

Co-processor exception description. This is a pointer to a string
and must be sufficient to hold 128 characters.

**Format:** 

`void EVE_LIB_GetCoProException(char *desc)`

**Parameters:**

- *desc* - buffer to receive the text of the exception description.

#### Continue with EVE_LIB_* API commands


### Creating screens and executing commands

The API Layer provides functions to begin and end lists of co-processor commands. The co-processor commands must be preceded and followed by co-processor management functions. 

#### Writing DL Instructions and Co-Processor Commands

Using EVE commands via the co-processor requires some data formatting to convert the parameters of the command into the correct hex values to be sent as well as keeping track of the number of bytes sent to update the write pointer correctly. Some commands also require padding to make their total size including parameters a multiple of 4 bytes. The functions in EVE_API hide this from the main application.

#### Beginning and Ending Co-Processor Lists

All co-processor lists must begin with a call to `EVE_LIB_BeginCoProList()`. 
If any display list items or co-processor commands which use the display list are to be added then a call to `EVE_CMD_DLSTART()` is required immediately after this.

For the avoidance of doubt, commands that only read or write registers, read or write memory, access flash or access the SD card do not require the `EVE_CMD_DLSTART()` call.

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
A call to `EVE_LIB_AwaitCoProEmpty()` is implied in the call to `EVE_LIB_BeginCoProList()`. Therefore it is not necessary to wait at the end of the co-processor
list for the completion of the commands allowing program to perform other tasks not related to programming the EVE device.

The `EVE_LIB_AwaitCoProEmpty()` function will return zero if the co-processor commands have run successfully. If there was an error with a co-processor command or data used by the co-processor then an exception can be raised which will require the application to handle. The Programming Guide for each generation details the actions required when this occurs. See the section called "Coprocessor Faults" or "Fault Scenarios". 

On EVE API 3, 4 and 5 there is a text message generated by the co-processor with a brief description of the fault. This message can be obtained with the `EVE_LIB_GetCoProException()` function.

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

#### Executing a Single Co-Processor Command

When just executing a co-processor command (for example calling CMD_SETROTATE during set-up of the application to set the screen orientation) then the following can be used:

```c
    EVE_LIB_BeginCoProList(); // CS low and send address in RAM_CMD 
    EVE_CMD_DLSTART(); // When executed, EVE will begin a new DL

    EVE_CMD_SETROTATE(2);

    EVE_LIB_EndCoProList(); // CS high
    EVE_LIB_AwaitCoProEmpty(); // Wait for FIFO to be finish
```

#### Co-Processor Lists of more than 4K Size

The examples above use burst writes (CS low, write address, stream data holding CS low, CS high). 

Therefore, no register writes should be carried out in the middle as this would interrupt the burst. A list can however be created in more than one section as shown below. This is also useful if a list consists of more than (4K-4) bytes. In this latter case the list would be written in smaller sections, each section being executed to create more space in the RAM_CMD FIFO before the next section is sent.

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

The API function `EVE_LIB_GetCoProSpace()` can be used to check if there is sufficient space available in the co-processor for further commands to be sent. The command will not stop and restart the co-processor lists as in the example above but will pause the SPI transfer to perform a register read before resuming another transfer.

#### Profiling the Co-processor List

Setting the `EVE_COPROC_PROFILE` macro will enable code that can count the number of bytes sent to the co-processor. This is useful to find out the size of each co-processor list.

It is initilised using `EVE_LIB_BeginCoProProfile()` at the beginning of a list to measure. The call to `EVE_LIB_GetCoProProfile()` will return the number of bytes written since the list profiling was initialised.

This feature can be used in conjunction with `EVE_LIB_GetCoProSpace()` to predict the size of the co-processor fullness.

Enabling the macro will add one 16-bit storage variable to the compiled project.

#### Limitations in RAM_DL and RAM_CMD

It is important to note that the overall limit of 8K for the generated RAM_DL list still applies, even if lists are sent in multiple sections. It is also important to bear in mind that the size of a co-processor command is not always the same as the size of the resulting RAM_DL instructions which the co-processor generates from the commands.

For example, the CMD_BUTTON uses 16 bytes of RAM_CMD plus the size of the string (plus any string arguments in BT81x) for the command, but the graphic operations in RAM_DL which the co-processor creates to render the button will be larger than this. The 8K RAM_DL limit does not therefore mean that 8K of co-processor commands can be used in one list.

REG_CMD_DL indicates the next available location in RAM_DL and so after executing a list commands (but before the swap) this register can be used to check how full RAM_DL is. The value read will be between 0 and 8191 with 8191 indicating the RAM_DL is full. 

The value of REG_CMD_DL is read after executing the commands above but before the swap is executed. The swap is sent using a separate transaction (beginning with `EVE_LIB_BeginCoProList()` and ending with `EVE_LIB_EndCoProList()` and `EVE_LIB_AwaitCoProEmpty()` ) because a register read or write cannot take place whilst an existing SPI transaction (burst write or read) is in progress.  Note that in this example the `EVE_LIB_Read16` is used and will work on EVE APIs 1 to 4, on EVE 5 only 32-bit reads and writes are supported.

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

These functions allow burst writes to be performed to RAM_G and RAM_CMD. Data bursts must be less than or equal to 65535 bytes, if larger bursts are required then they must be split into smaller sections. The HAL layer and MCU layer will further limit transfers to `HAL_MAX_CHUNK_SIZE` bytes.

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
This function sends a string of characters and is used by commands such as CMD_TEXT, CMD_BUTTON and CMD_TOGGLE which all use text strings. This function takes care of the extra padding which is required as all EVE commands must be 32-bit aligned. Therefore, depending on the length of the string (plus the necessary null character to terminate it) then between one and three extra 00 bytes are added to pad the command to be a multiple of 4 bytes. The main application can therefore send strings without needing to consider the padding. 

#### Handling Interrupts

The interrupt register `REG_INT_FLAGS` is provided to allow an application to see if one of several interrupt events are flagged. These can be polled by reading the register. However, the register is automatically cleared on each read. 

The API provides a method for accessing the register and preserving any tested flags for later testing. The `EVE_LIB_GetInterrupt()` function is provided to load and store the current bits set to keep a set of flags set in a global variable. There is a mask value as a parameter to the function which is used to test the set bits. Once the bits have been tested in the global variable they can be cleared.

For example, if a key press was detected and the bit set in the register during the period the application was waiting for a command buffer empty event the API can be queried with `EVE_LIB_GetInterrupt(EVE_INT_CMD_EMPTY)`. The `EVE_INT_TOUCH` bit would be unaffected and the application could later independently test the command for that event.

Enabling the macro will add one 8-bit storage variable to the compiled project.

#### Accessing the INT# line

The optional INT# line is provided for the EVE device to signal to the host MCU that an event has occurred. The `REG_INT_FLAGS` register holds a flag of all interrupts that are pending. If the corresponding bit in the `REG_INT_MASK` register is set then the EVE device will set the INT# line low (active). This signal can be used when single-channel SPI is in use. If Quad SPI is being used then this signal is used as a data line instead.

This status can be accessed from the EVE API with the `EVE_LIB_Int()` function. A non-zero value indicates that the INT# line is asserted.
