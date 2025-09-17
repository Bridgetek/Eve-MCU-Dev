# EVE-MCU-Dev Ports

[Back](../README.md)

## Porting EVE-MCU-Dev

The EVE-MCU-Dev library is intended for simple porting to MCU and embedded environments. There are several common target types which are provided with this library:

| Port Name | Port Directory | Variant | Test Status |
| --- | --- | --- | --- |
|Bridgetek FT90x | [eve_arch_ft9xx](eve_arch_ft9xx/README.md) | FT90X | Pass |
|Bridgetek FT93x | [eve_arch_ft9xx](eve_arch_ft9xx/README.md) | FT93X | Pass |
|Beaglebone | [eve_arch_beaglebone](eve_arch_beaglebone/README.md) | Beaglebone Black | Requires Update |
|Expressif ESP32 | [eve_arch_esp32](eve_arch_esp32/README.md) | SparkFun ESP32 Thing | Pass |
|TI MSP430 | [eve_arch_msp](eve_arch_msp/README.md) | MSP430G2553 | Compile Only |
|TI MSPM0 | [eve_arch_msp](eve_arch_msp/README.md) | MSPM0G3519 | Compile Only |
|Microchip PIC18F | [eve_arch_msp430](eve_arch_pic/README.md) | N/A | Requires Update |
|ST STM32 (Keil) | [eve_arch_stm32](eve_arch_stm32/README.md) | N/A | Requires Update |
|ST STM32Cube | [eve_arch_stm32](eve_arch_stm32/README.md) | N/A | Requires Update |
|Raspberry Pi | [eve_arch_rpi](eve_arch_rpi/README.md#hardware-raspberry-pi) | Raspberry Pi Model 0 to 5 | Pass |
|Raspberry Pi Pico (SDK and VS Code) | [eve_arch_rpi](eve_arch_rpi/README.md#hardware-rp2040) | RP2040 | Pass |
|Generic using libMPSSE  | [eve_libmpsse](eve_libmpsse/README.md) | FT4232H and FT232H | Pass |
|Generic using libFT4222  | [eve_libft4222](eve_libft4222/README.md) | FT4222H | Pass |

- Pass: Has been compiled with the latest toolchains and run on hardware across of EVE generations.
- Under Test: Compiles with the latest toolchain but hardware testing is ongoing.
- Compile Only: Latest toolchain has been used to successfully compile code for EVE generations. No hardware testing carried out with new toolchain. Previous version of toolchain has been used for hardware testing.
- Requires Update: Previous version of toolchain has been used for hardware testing.

For most parts a generic variant of the device has been chosen. The specific device variant is shown in the table above.