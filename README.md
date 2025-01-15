# EVE-MCU-Examples-BRT_AN_025

The code is in the process of being updated to support the BT82x devices (EVE generation 5). Not all platforms have been coded and tested for BT82x. Outstanding work will be completed as time allows.

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
