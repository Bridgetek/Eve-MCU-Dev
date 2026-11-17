# EVE-MCU-Dev Release Notes

**Release:** `2.0` 18th November 2026

**Previous Release:** [`1.1`](https://github.com/Bridgetek/Eve-MCU-Dev/releases/tag/1.1) 21st July 2026  

These notes summarise changes since the **1.1** release.

## Overview

This update restructures the library's headers and configuration, improves co-processor command handling, expands BT820 functionality, and adds platform and example support. Build dependencies, documentation, and automated regression testing have also been substantially revised. Backward compatibility has been maintained as far as possible.

## New Features and Enhancements

### Library structure and configuration

- Consolidated generation-specific definitions into common header files, including `EVE_commands.h` and `EVE_registers.h`.
- Separated application configuration, shared definitions, and derived settings into `EVE_config.h`, `EVE_defs.h`, and `EVE_settings.h`.
- Enabled applications to supply their own configuration file `EVE_config.h` through the include search path.
- Reduced dependencies between the API, HAL, and MCU layers, adding `MCU_SetSPIMode()`, `Platform_SetSPIMode()`, and `HAL_GetInterrupt()`.
- Centralised diagnostic output in `EVE_debug.h`.

### Co-processor management

- Added timeout-based waiting through `EVE_LIB_AwaitCoProEmptyTimeout()` and recovery through `EVE_LIB_RecoverCoPro()`.
- Added command-buffer space reporting and optional profiling of command lists and display lists.
- Added optional use of the EVE `INT#` line for detecting co-processor list completion on supported ports.
- Corrected command FIFO wraparound handling when using the `REG_CMD_WRITE` transfer method and improved exception reporting for EVE API levels 3 and 4. 

### BT820 support

- Added BT82x command implementations for `COPYLIST`, `PLAYWAV`, DDR startup/shutdown, context save/restore, conditional skipping, and watchdog control.
- Added configurable BT820 LVDS scanout modes, channel selection, and output formats.
- Moved BT82x patches into a dedicated extension.

### Displays and touch support

- Extended BT817/BT818 pixel-clock and adaptive frame-rate configuration.
- Expanded module and panel presets and revised WXGA and WUXGA timing settings.
- Moved custom touch firmware, and LCD driver initialisation into dedicated extensions, including support for IDM2040-21R panel initialisation. 

### Platforms and examples

- Added Raspberry Pi Pico 2/RP2350 support through the shared Pico SDK port.
- Added an Infineon PSoC 6 port and Eclipse example for CY8CPROTO-062S2-43439.
- Added PlatformIO support for Arduino and ESP-IDF configurations for the simple example.
- Expanded Emulator and Pico example coverage.
- Added **Colour Picker** and **Spaced Invaders** examples, plus a radar plot widget in Data Visualiser.
- Moved calibration storage and platform timing into reusable snippets. Added EEPROM calibration storage where supported and safer flash programming for Pico SDK builds. 

## Fixes

- Fixed a potential buffer overread when sending padded strings in `EVE_LIB_SendString()`.
- Corrected BT820 startup and system-clock command sequencing, including an FT4222 host-command interaction.
- Corrected the BT82x `SDBLOCKREAD` opcode.
- Applied duplicate `DISPLAY` command handling for earlier EVE generations in line with the referenced erratum.
- Corrected command availability and signatures: enabled `GRADIENTA` for EVE2 and added the BT820 options parameter to `VIDEOSTART`.
- Corrected integer packing in several command implementations, image decoding on EVE3, and example touch-processing logic. 

## Build, Testing, and Documentation

- Consolidated CMake configuration into shared example and platform files.
- Added Git submodules for the EVE emulator and FTDI dependencies; updated the emulator to **v5.3.35**.
- Updated Visual Studio projects and removed legacy Win32 configurations.
- Expanded test coverage and added reference-output regression checks for boot sequences, API commands, and selected examples.
- Added **QUICKSTART.md**, **PORTING.md**, and a separate **API.md** documents.
- Revised wiring guides in port and overview README.md files. 
- Added repository-local AI assistance instructions. 

## Upgrade Notes

- Update configuration macros to the `EVE_*` naming convention—for example, `FT8XX_TYPE` → `EVE_DEVICE`, `MODULE_TYPE` → `EVE_MODULE`, and `QUADSPI_ENABLE` → `EVE_QSPI_ENABLE`. Compatibility aliases remain for deprecated settings.
- Use Platform macro `PLATFORM_PICO_SDK` for Pico projects but retain `PLATFORM_RP2040` for backward compatibility. Port file is renamed from `EVE_MCU_RP2040.c` to `EVE_MCU_PICO_SDK.c` for Pico projects.
- Replace misspelled API names `EVE_CMD_FLASHDETATCH()` and `EVE_PALLETE_SOURCE_H()` with `EVE_CMD_FLASHDETACH()` and `EVE_PALETTE_SOURCE_H()`.
- For BT820, pass an options argument to `EVE_CMD_VIDEOSTART()`.
- Restart the co-processor list before submitting further commands after `EVE_LIB_WriteDataToCMD()`.
- NOTE: Initialise the required submodules when updating an existing checkout.
