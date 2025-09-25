# EVE-MCU-Dev

This library and ports allow a variety of hardware to communicate with FT8xx and BT8xx devices: embedded MCUs using their native SPI hardware; Linux PCs using SPI character devices; PCs using FT4222H or MPSSE USB devices. This library is intended to provide a **C** library for embedded designs.

The application note document for this library is linked on the Bridgetek Website in [Home / Software Examples / EVE Examples / Portable EVE Library](https://brtchip.com/software-examples/eve-examples-2/). This library is covered by Application Note [BRT_AN_025 EVE Portable MCU Example](https://brtchip.com/wp-content/uploads/2024/04/BRT_AN_025_EVE_Portable_MCU_Example-R.pdf)

## Ports

The supported platforms are listed in the [ports/README.md](ports/README.md) file. 

The source code for each platform is stored in the [ports](ports) directory. Each source file in each ports folder is guarded by one of the PLATFORM_<i>xxx</i> macros, USE_<i>xxx</i> macros, or a develoment environment specific macro. This way all the files in the ports directory can be loaded into a compiler and ignored if they are not relevant.

## Example Code

There are example projects for many each supported platform. The [examples/README.md](examples/README.md) file has details on each of the included examples.

The [simple](examples/simple) example has build environments for all platforms and forms the basis of other examples that are provided. Build instructions are included in the simple example directory in the file [simple/README.md](examples/simple/README.md).

## EVE Device and Panel Selection

The library __must__ be built for the correct EVE device and panel type. This is defined in the file [include/EVE_config.h](include/EVE_config.h).

### EVE Device Selection

The available EVE devices are:
- EVE API 1
  - FT800 
  - FT801 
- EVE API 2
  - FT810 
  - FT811 
  - FT812 
  - FT813 
  - BT880 
  - BT881 
  - BT882 
  - BT883 
- EVE API 3
  - BT815 
  - BT816 
- EVE API 4
  - BT817 
  - BT818 
- EVE API 5
  - BT820 

The EVE device to target is set in the file `EVE_config.h` using the macro `FT8XX_TYPE` or `EVE_API`.

If the `FT8XX_TYPE` macro is used then the "FT" or "BT" part number of the device is set. This line will set a BT820 device and EVE API 5 will be selected automatically.
```
#define FT8XX_TYPE BT820
```
If `EVE_API` is used this will override any `FT8XX_TYPE` values and a number from 1 to 5 is used. For EVE API 2 a subtype of the API is set in the `EVE_SUB_API` macro. So for an FT813 device the following can be used:
```
#define EVE_API 2
#define EVE_SUB_API 1
```
**The default in the distribution will be a BT820 device**.

Note that the example programs will take the `EVE_config.h` file from the `include` directory.

### Panel Selection

The panel dimensions to use are set in the file `EVE_config.h` using the macro `DISPLAY_RES`. 

Various standard panels are included, if a new panel is needed then the settings can be derived from the panel specifications or contact Bridgetek Support for advice.

The following are included in the distribution:
- WQVGA *480x272* (VM800B with 5 or 4.3 inch display)
- WVGA *800x480* (ME813A-WH50C or VM816)
- WSVGA *1024x600* (ME817EV with 7 inch display)
- WXGA *1280x800* (ME817EV with 10.1 inch display)
- HD *1920x1080* (15 inch high definition display)
- WUXGA *1920x1200* (10 inch high definition display)

This line will set the panel to a ME-813A-WH50C (800 x 400).
```
#define DISPLAY_RES WVGA
```

**The default in the distribution will be a WUXGA panel**.

## EVE Module Connection

There are 2 standard connectors for EVE modules used by BridgeTek. 

The wiring colours in the section for each connection are defined in the following table.

| Colour | EVE Signal |
| --- | --- |
| Blue | SCK |
| Green | MOSI |
| Yellow | MISO |
| Orange | CS# |
| Red | PD# |
| Brown | GND |

### Through-Board 2x8 Pins

This connector is a through-board connector 2x8 pin with 2.54mm spacing commonly found on the "ME" range of boards. These are designed with longer pins that can be used with the MM900EVxB FT9XX boards to mount the MCU board ontop of the EVE module.

| Pin | EVE Signal | Pin | EVE Signal |
| --- | --- | --- | --- |
| 1 | N/C | 2 | N/C |
| 3 | N/C | 4 | PD# |
| 5 | GND | 6 | N/C |
| 7 | 5V | 8 | N/C |
| 9 | N/C | 10 | N/C |
| 11 | N/C | 12 | N/C |
| 13 | MOSI | 14 | MISO |
| 15 | CS# | 16 | SCK |

The 2x8 header can be connected as in the following picture.

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

The 1x10 header can be connected as in the following picture.

![Wiring for 1x10 Header](docs/header1x10.png)

