# EVE-MCU-Dev Ports

[Back](../README.md)

## Porting EVE-MCU-Dev

The EVE-MCU-Dev library is intended for simple porting to MCU and embedded environments. There are several common target types which are provided with this library:

| Port Name | Platform Macro | Port Directory | Variant | Test Status (1) |
| --- | --- | --- | --- | --- |
| Arduino | `ARDUINO` (2) | [eve_arch_arduino](eve_arch_arduino/README.md) | Arduino Leonardo and Arduino Zero | Pass |
| Beaglebone | `PLATFORM_BEAGLEBONE` | [eve_arch_beaglebone](eve_arch_beaglebone/README.md) | Beaglebone Black | Under Test |
| Expressif ESP32 | `PLATFORM_ESP32` | [eve_arch_esp32](eve_arch_esp32/README.md) | SparkFun ESP32 Thing | Pass |
| Bridgetek FT9xx | `PLATFORM_FT9XX` | [eve_arch_ft9xx](eve_arch_ft9xx/README.md) | FT90X and FT93X | Pass |
| TI MSP430 | `PLATFORM_MSP430` | [eve_arch_msp](eve_arch_msp/README.md) | MSP430G2553 | Under Test |
| TI MSPM0 | `PLATFORM_MSPM0` | [eve_arch_msp](eve_arch_msp/README.md) | MSPM0G3519 | Under Test |
| Microchip PIC18F | `PLATFORM_PIC` | [eve_arch_pic](eve_arch_pic/README.md) | N/A | Under Test |
| Raspberry Pi | `PLATFORM_RASPBERRYPI` | [eve_arch_rpi](eve_arch_rpi/README.md#hardware-raspberry-pi) | Raspberry Pi Model 0 to 5 | Pass |
| Raspberry Pi Pico (SDK and VS Code) | `PLATFORM_RP2040` | [eve_arch_rpi](eve_arch_rpi/README.md#hardware-rp2040) | RP2040 | Pass |
| ST STM32 (Keil) | `PLATFORM_STM32` | [eve_arch_stm32](eve_arch_stm32/README.md) | N/A | Pass |
| ST STM32Cube | `PLATFORM_STM32CUBE` | [eve_arch_stm32](eve_arch_stm32/README.md) | N/A | Pass |
| Generic using libMPSSE | `USE_MPSSE` (3) | [eve_libmpsse](eve_libmpsse/README.md) | FT4232H and FT232H | Pass |
| Generic using libFT4222 | `USE_FT4222` (3) | [eve_libft4222](eve_libft4222/README.md) | FT4222H | Pass |

- (1) Key for Test Status
  - Pass: Has been compiled with the latest toolchains and run on hardware across of EVE generations.
  - Under Test: Compiles with the latest toolchain but hardware testing is ongoing.
- (2) The Arduino port uses the build-in `ARDUINO` macro from the Arduino IDE and toolchain.
- (3) The macro must be set to a value rather then just be defined. This is the index of the FT4222H device to use when the USB is scanned.

For most parts a generic variant of the device has been chosen. The specific device variant is shown in the table above.

Note: The Rasberrry Pi and BeagleBone Black ports are based on Linux builds and are suitable for most embedded Linux platforms by modifying the GPIO pins and device names to match the CPU and board package.
