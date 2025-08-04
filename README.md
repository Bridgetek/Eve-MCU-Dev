# EVE-MCU-Dev

This library and ports allow a variety of hardware to communicate with FT8xx and BT8xx devices: embedded MCUs using their native SPI hardware; Linux PCs using SPI character devices; PCs using FT4222H or MPSSE USB devices. This library is intended to provide a **C** library for embedded designs.

The code is in the process of being updated to support the **BT82x** devices (EVE generation 5). Not all platforms have been coded and tested for BT82x. Outstanding work will be completed as time allows.

The application note document for this library is linked on the Bridgetek Website in (Home / Software Examples / EVE Examples / Portable EVE Library)[https://brtchip.com/software-examples/eve-examples-2/]. The document is (BRT_AN_025 EVE Portable MCU Example)[https://brtchip.com/wp-content/uploads/2024/04/BRT_AN_025_EVE_Portable_MCU_Example-R.pdf]

## Ports

### PLATFORM Macro

The PLATFORM macro sets the port files to be used by the compiler. The options are as follows:

| Port Name | PLATFORM | Port Directory | BT82x Support |
| --- | --- | --- | --- |
|Bridgetek FT90x | PLATFORM_FT9XX | eve_arch_ft9xx | Yes |
|Bridgetek FT93x | PLATFORM_FT9XX | eve_arch_ft9xx | Yes |
|Beaglebone Black | PLATFORM_BEAGLEBONE | eve_arch_beaglebone | No |
|Expressif ESP32 | PLATFORM_ESP32 | eve_arch_esp32 | No |
|TI MSP430 | PLATFORM_MSP430 | eve_arch_msp430 | No |
|Microchip PIC18F | PLATFORM_PIC | eve_arch_pic | No |
|ST STM32 (Keil) | PLATFORM_STM32 | eve_arch_stm32 | Yes |
|ST STM32Cube | PLATFORM_STM32CUBE | eve_arch_stm32cube | Yes |
|Raspberry Pi | PLATFORM_RASPBERRYPI | eve_arch_rpi | Yes |
|Raspberry Pi Pico | PLATFORM_RP2040 | eve_arch_rpi | Yes |
|Generic using libMPSSE | USE_MPSSE | eve_libmpsse | Yes |
|Generic using libFT4222 | USE_FT4222 | eve_libft4222 | Yes |

The C source files are in the [ports](ports/) directory. Each source file in each ports folder is guarded by one of the PLATFORM_<i>xxx</i> macros.

The Rasberrry Pi platform is suitable for generic Linux by modifying the GPIO pins and device names to match the CPU and board package.

## Example Code

There are example projects for many MCUs and platforms. These include this repository as a submodule within the required code for the platform.

Build instructions will be included in the example repositories.

### Microcontrollers

| MCU | Example Repository | Build Environment |
| --- | --- | --- |
|Bridgetek FT90x/FT93x | examples/simple/ft900 | ft32-elf-gcc using FT9xx Toolchain IDE or makefile |
|Raspberry Pi Pico | examples/simple/pico | CMake, gcc |
|Beaglebone Black | examples/simple/BeagleBone | |
|Expressif ESP32 | examples/simple/ESP32 | |
|TI MSP430 | examples/simple/MSP430 | |
|Microchip PIC18F | examples/simple/PIC18F | |
|ST STM32 (Keil) | examples/simple/STM32 | Keil IDE and STM32CubeMX |
|ST STM32Cube | examples/simple/STM32CUBE | STM32Cube IDE and STM32CubeMX |
</table>

### Platforms
| Platform | Example Repository | Build Environment |
| --- | --- | --- |
|Raspberry Pi | examples/simple/raspberry_pi | makefile/gcc |
|Generic using libMPSSE | examples/simple/libmpsse | makefile/gcc or VisualStudio |
|Generic using libFT4222 | examples/simple/libft4222 | makefile/gcc or VisualStudio |

## EVE and Panel Selection

The library must be built for the correct EVE device and panel type. 

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

The panel dimensions to use are set in the file `EVE_config.h` using the macro `DISPLAY_RES`. Various standard panels are included, if a new panel is needed then the settings can be derived from the panel specificaitions or contact Bridgetek Support for advice.

The following are included in the distribution:
- WQVGA *480x272* (VM800B with 5 or 4.3 inch display)
- WVGA *800x480* (ME813A-WH50C or VM816)
- WSVGA *1024x600* (ME817EV with 7 inch display)
- WXGA *1280x800* (ME817EV with 10.1 inch display)
- HD *1920x1080* (15 inch high definition display)
- WUXGA *1920x1200* (10 inch high definition display)

**The default in the distribution will be a WUXGA panel**.