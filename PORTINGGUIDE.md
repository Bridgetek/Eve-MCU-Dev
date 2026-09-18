# EVE-MCU-Dev Porting Guide

This porting guide describes the steps required to add support for a new MCU or host platform to the `EVE-MCU-Dev` library.

**Reference baseline:** The [`development` branch][development], reviewed on 18 September 2026. The source links in this guide follow that branch and may change. Record the commit used for your implementation before starting.

## Introduction

The objective of an MCU port is to connect the common EVE library to the target's SPI, GPIO and timing facilities, while keeping EVE commands and register handling in the existing library layers. Begin with a basic, blocking, single-SPI implementation and a known display configuration. Add performance improvements and optional features only after communication and display output have been verified.

This guide adapts the staged approach in [BRT_AN_062: Porting BRT_AN_025 to NXP MCU][an062]: establish the development environment, test the low-level interface, integrate the common library, and run an example. Function names, source files and build instructions below are based on the current repository rather than the older NXP project.

### Scope

The instructions primarily cover ports implementing [`include/MCU.h`][mcu-h] and using [`source/EVE_HAL.c`][hal-c]. Some host-interface ports also use this arrangement. The Linux SPI-device path uses `Platform.h` and `EVE_HAL_Linux.c` instead and is not a drop-in variation of the MCU implementation described here. See the [repository overview][readme] for the wider architecture.

This guide does not replace the target MCU's SDK documentation, the selected EVE device's documentation, or the display module's schematic. Adding an unsupported display panel, touch controller or EVE device is separate from adapting the MCU transport. The templates are starting points: SDK-specific code must be supplied and validated on the target hardware.

## Contents

- [1. Establish the starting point](#1-establish-the-starting-point)
- [2. Define the port and hardware interface](#2-define-the-port-and-hardware-interface)
- [3. Prove the MCU SPI and GPIO operations](#3-prove-the-mcu-spi-and-gpio-operations)
- [4. Implement the MCU interface](#4-implement-the-mcu-interface)
- [5. Select the EVE device and display configuration](#5-select-the-eve-device-and-display-configuration)
- [6. Integrate the source files and build configuration](#6-integrate-the-source-files-and-build-configuration)
- [7. Connect the application entry point](#7-connect-the-application-entry-point)
- [8. Bring up the display in stages](#8-bring-up-the-display-in-stages)
- [9. Add optional features](#9-add-optional-features)
- [10. Troubleshooting](#10-troubleshooting)
- [11. Validate and document the completed port](#11-validate-and-document-the-completed-port)
- [Source reference index](#source-reference-index)

## 1. Establish the starting point

### Obtain a consistent checkout

Work from one revision of the library. Do not combine an older `MCU.h`, application-note source files and a newer HAL.

```sh
git clone --branch development --recurse-submodules https://github.com/Bridgetek/Eve-MCU-Dev.git
cd Eve-MCU-Dev
git rev-parse HEAD
```

For an existing checkout, update its submodules to the revisions recorded by that checkout:

```sh
git submodule update --init --recursive
```

Record the repository commit, compiler, SDK, IDE or CMake version, board revision and display module. This makes later failures reproducible. External dependencies are port-dependent; inspect the [ports documentation][ports-readme] rather than assuming every MCU needs the same host libraries.

### Start from a working board project

Create or import a vendor-supported project for the target board. Build, download and run a simple LED or serial-output test before introducing EVE. Confirm that the debugger, system clock, GPIO configuration and a usable time base work.

Retain the necessary startup code, linker script, clock configuration and SDK initialisation. Replacing a generated `main.c` with a donor example can accidentally remove these prerequisites.

Choose an existing port with a similar SDK or peripheral interface as a reference. The [RP2040 implementation][rp2040-c] demonstrates a direct blocking SPI implementation; the [STM32 SPI implementation][stm32-spi-c] demonstrates another SDK integration. Copy the organisation and required interface, not the donor's pins, clock frequencies or flash addresses. The current [simple-example README][simple-readme] marks the NXP K64 build environment as incomplete, so the older NXP application note should not be treated as a verified current build recipe.

**Checkpoint:** A standalone board project runs reliably, and the library revision is recorded.

## 2. Define the port and hardware interface

### Keep the implementation boundaries clear

The normal call path is:

```text
Application / example
        |
        v
EVE API                  EVE.h / source/EVE_API.c
        |
        v
EVE HAL                  HAL.h / source/EVE_HAL.c
        |
        v
MCU interface            MCU.h / new MCU implementation
        |
        v
Vendor SDK / SPI / GPIO / timing hardware
```

This is a runtime/interface overview, not a header-include diagram. The MCU implementation supplies transport and host operations. It should not generate display lists, interpret EVE registers, or call back into the EVE API to perform a transfer. `MCU.h` already includes `EVE_settings.h` for derived build-time configuration; an ordinary MCU transport implementation does not need `EVE.h` or `HAL.h`. [Sources: library architecture][readme], [MCU interface][mcu-h].

A proposed layout for a new port is:

```text
ports/
  eve_arch_newmcu/
    EVE_MCU_NEWMCU.c
    newmcu.cmake                 # Optional: CMake integration
    README.md
examples/
  simple/
    newmcu/
      CMakeLists.txt             # Or the vendor IDE project
      EVE_config.h               # Optional: application-local configuration
      main/
        main.c
      board/
        board_support.c          # Illustrative board/SDK glue
        board_support.h
```

`newmcu`, `PLATFORM_NEWMCU` and `board_support.*` are placeholders introduced by this guide, not existing repository interfaces. Use a distinct platform macro consistently in the build and the new source file. Do not define another MCU's macro merely to make its code compile.

### Establish the connections

Create a pin-assignment table for the actual board and module before writing the port.

| EVE connection | MCU-side requirement |
| --- | --- |
| SCK | SPI master clock output. |
| MOSI / IO0 | SPI transmit connection; bidirectional data line when the selected multi-line interface requires it. |
| MISO / IO1 | SPI receive connection; bidirectional data line when required. |
| IO2 and IO3 | Additional data connections when Quad SPI is used. |
| CS# | Independently controllable active-low chip select, normally a GPIO output. |
| PD# | Active-low power-down/reset control, normally a GPIO output. |
| INT# | Optional input for interrupt-based completion or application interrupt handling. Do not configure it as an MCU output. |
| Supply and ground | Correct module supply, adequate current capability and a common signal ground. |

Check the module's schematic and electrical limits. A module powered from 5 V does not necessarily accept 5 V on its SPI or control inputs. Verify required pull-ups, level translation, power sequencing and the treatment of unused data pins against the selected hardware documentation. These are hardware-specific requirements, not values to copy from the NXP wiring diagram. [Source: hardware bring-up approach in BRT_AN_062][an062].

For the first implementation, choose SPI master mode, SPI Mode 0, 8-bit transfers and MSB-first bit order. A conservative initial clock such as 1 MHz is a useful starting point, as demonstrated by the [RP2040 port][rp2040-c]. Do not assume that its later operating speed is suitable for another board or EVE device.

**Checkpoint:** Every signal has a documented pin, direction, voltage and initial state.

## 3. Prove the MCU SPI and GPIO operations

Test the vendor SPI and GPIO routines before debugging the full EVE initialisation sequence. This preserves the useful separation between peripheral testing and library integration used in [BRT_AN_062][an062].

With EVE disconnected or kept deselected, transmit a recognisable test pattern and inspect SCK, MOSI and CS# with a logic analyser. Verify the selected pins, idle clock state, clock frequency, bit order and exact byte count. A loopback test can check receive handling when the wiring and disconnected peripheral arrangement permit it; remove the loopback before connecting EVE. Do not send arbitrary test patterns to a selected EVE device as though they were valid commands.

Then verify the intended transaction boundary: assert CS#, make several successive SDK transfers, and deassert CS# only after the final bit has left the peripheral. CS# must not pulse high between the address and data phases simply because these use separate function calls.

Also check that a read generates clock pulses, that unused received data from writes cannot accumulate into an RX overrun, and that the delay/time functions work at the final MCU system-clock setting.

**Checkpoint:** The MCU can clock bytes in both directions while preserving a manually controlled transaction boundary.

## 4. Implement the MCU interface

Use the declarations in the checkout's [`include/MCU.h`][mcu-h] as the interface checklist. The older application note's combined `MCU_SPIReadWrite8()` is not a replacement for the current public read and write functions. A private byte-exchange helper is still a convenient implementation technique.

### Required function groups

| Group | Functions to implement | Responsibility |
| --- | --- | --- |
| Lifecycle | `int MCU_Init(void)`, `int MCU_Setup(void)`, `int MCU_Deinit(void)` | Configure, adjust and release the host interface. Return `0` on success and `-1` on failure. |
| Control pins | `MCU_CSlow()`, `MCU_CShigh()`, `MCU_PDlow()`, `MCU_PDhigh()` | Apply the named physical pin levels. |
| Interrupt input | `int MCU_Int(void)` | Return the physical INT# level: `0` when asserted and non-zero when deasserted. Explicitly reject unsupported interrupt use; see **Section 9**. |
| Block transfers | `MCU_SPIWrite(const uint8_t *, uint32_t)`, `MCU_SPIRead(uint8_t *, uint32_t)` | Transfer the requested number of bytes within the current transaction. |
| Scalar transfers | `MCU_SPIWrite8()`, `MCU_SPIWrite16()`, `MCU_SPIWrite24()`, `MCU_SPIWrite32()`, `MCU_SPIRead8()`, `MCU_SPIRead16()`, `MCU_SPIRead32()` | Supply the fixed-size operations used by the HAL. |
| Timing | `MCU_Delay_20ms()`, `MCU_Delay_500ms()`, `uint32_t MCU_Time_ms(void)` | Supply minimum delays and a monotonic millisecond count. |
| Host to wire byte order | `MCU_htobe16()`, `MCU_htobe32()`, `MCU_htole16()`, `MCU_htole32()` | Convert host values to the specified byte order. |
| Wire to host byte order | `MCU_be16toh()`, `MCU_be32toh()`, `MCU_le16toh()`, `MCU_le32toh()` | Convert the specified byte order to host values. |
| Optional interface width | `int MCU_SetSPIMode(uint8_t mode)` | Configure the host's SPI width when `EVE_QSPI_ENABLE` is defined. |

There is no active `MCU_SPIRead24()` requirement. Implement the baseline interface rather than relying on the first selected example to exercise every function. [Source: current declarations][mcu-h].

### Initialisation, setup and shutdown

`MCU_Init()` should prepare the SPI peripheral and control pins for EVE boot. Configure `CS#` inactive and `PD#` inactive, configure `INT#` as an input when connected, and establish the conservative SPI settings. Where the MCU allows it, preload inactive GPIO output levels before enabling their output drivers to avoid unwanted pulses.

Keep EVE reset timing, host commands and register initialisation in the existing HAL. `MCU_Setup()` runs after EVE has booted and can increase the host SPI clock; it may simply return success while the first implementation retains the initial speed. `MCU_Deinit()` should leave the bus safe and release resources owned by the port. Avoid resetting a shared peripheral that another device still uses. [Sources: lifecycle declarations][mcu-h], [reference implementation][rp2040-c].

Place any source-level platform guard before vendor-specific includes:

```c
#if defined(PLATFORM_NEWMCU)

#include "MCU.h"
#include "board_support.h"  /* Project-specific SPI, GPIO and timing helpers. */

#if defined(EVE_QSPI_ENABLE)
#error "This initial port supports single SPI only."
#endif

/* Implement the MCU interface here. */

#endif /* defined(PLATFORM_NEWMCU) */
```

### Blocking transfers and chip-select ownership

The simplest implementation completes every transfer before returning. Neither `MCU_SPIWrite()` nor `MCU_SPIRead()` should independently assert or deassert `CS#`. The HAL surrounds related transfer calls with the control-pin operations. [Sources: HAL transfer implementation][hal-c], [RP2040 transfers][rp2040-c].

The following original adapter pattern assumes a project-specific `board_spi_exchange8()` that transmits one byte, consumes the received byte and waits for completion. That helper must be implemented with the target SDK; it is not an EVE-MCU-Dev function.

```c
void MCU_SPIWrite(const uint8_t *data, uint32_t length)
{
    while (length != 0u)
    {
        (void)board_spi_exchange8(*data++);
        --length;
    }
}

void MCU_SPIRead(uint8_t *data, uint32_t length)
{
    while (length != 0u)
    {
        *data++ = board_spi_exchange8(0u);
        --length;
    }
}
```

This deliberately prioritises correctness over throughput. A blocking SDK buffer transfer can replace the loop later. If the SDK has a smaller count type or maximum transfer size, split the request internally without changing `CS#` or truncating the `uint32_t` length. A zero-length request should do nothing.

Hardware-managed chip select is not inherently prohibited, but it must preserve the entire transaction across all the HAL's constituent calls. An SDK which always releases chip select after each buffer is unsuitable without adaptation. The [STM32 Quad SPI port][stm32-qspi-c] illustrates that a buffered transaction design is possible; it is a more complex reference than a direct blocking port.

The current transfer functions do not return an error status. Decide how SDK failures will be exposed through diagnostics and the application's fault-handling policy. A failed transfer must not be silently treated as a valid read simply because the interface is `void`.

### Scalar transfers and byte order

MSB-first SPI bit order is not the same as byte order within a multi-byte value. The HAL performs the protocol conversions. The scalar transport helpers should preserve the object-byte representation expected by that HAL, rather than applying an additional unconditional byte swap. This separation is visible in the [HAL address/data operations][hal-c] and [RP2040 scalar helpers][rp2040-c].

For example:

```c
void MCU_SPIWrite8(uint8_t value)
{
    MCU_SPIWrite(&value, sizeof(value));
}

uint8_t MCU_SPIRead8(void)
{
    uint8_t value = 0u;
    MCU_SPIRead(&value, sizeof(value));
    return value;
}

void MCU_SPIWrite16(uint16_t value)
{
    MCU_SPIWrite((const uint8_t *)&value, sizeof(value));
}

uint16_t MCU_SPIRead16(void)
{
    uint16_t value = 0u;
    MCU_SPIRead((uint8_t *)&value, sizeof(value));
    return value;
}

void MCU_SPIWrite24(uint32_t value)
{
    /* The HAL has prepared the address: send its first three object bytes. */
    MCU_SPIWrite((const uint8_t *)&value, 3u);
}

void MCU_SPIWrite32(uint32_t value)
{
    MCU_SPIWrite((const uint8_t *)&value, sizeof(value));
}

uint32_t MCU_SPIRead32(void)
{
    uint32_t value = 0u;
    MCU_SPIRead((uint8_t *)&value, sizeof(value));
    return value;
}
```

Implement all eight conversion functions with the correct behaviour for the target:

| Host representation | Host <-> little-endian | Host <-> big-endian |
| --- | --- | --- |
| Little-endian MCU | Leave the value unchanged. | Swap its bytes. |
| Big-endian MCU | Swap its bytes. | Leave the value unchanged. |

Use the compiler or SDK's supported byte-swap operations, or a verified portable implementation. Do not assume a donor port's no-op conversions apply to a different architecture. Use block transfers as byte streams; do not reinterpret a bitmap or command buffer as an array of values to be swapped.

### Timing

Make `MCU_Delay_20ms()` and `MCU_Delay_500ms()` wait for at least their stated durations. Implement `MCU_Time_ms()` from a running monotonic time source, not a variable incremented only when the application happens to call a delay. Its `uint32_t` value may wrap; elapsed-time calculations should use unsigned subtraction. [Source: timing interface][mcu-h].

Ensure the time base is already available wherever the functions can run. An interrupt-driven SDK delay can stop progressing if interrupts are disabled, and a delay calibrated before a clock change may no longer be correct. These conditions should be tested, not hidden by increasing arbitrary delay constants.

**Checkpoint:** The MCU interface builds without donor-platform dependencies, and its transfers, pin levels and timing have been checked independently.

## 5. Select the EVE device and display configuration

Start with a supported module or a known device/panel configuration. Changing both the MCU transport and an unverified display timing configuration makes faults harder to isolate.

The main selection values are `EVE_MODULE`, `EVE_DEVICE`, `EVE_PANEL`, `EVE_DISPLAY_RES` and `EVE_COPRO_METHOD`. A recognised module selection overrides the separate device, panel and resolution choices; a recognised panel selection determines the resolution. [`EVE_settings.h`][settings-h] derives the effective configuration from these values. Do not manually define derived API-selection macros to disguise a mismatched device.

For example, the following values describe a BT817/WVGA bring-up configuration, not a universal display selection:

```c
/* Illustrative values to apply to the corresponding configuration entries. */
#define EVE_MODULE       EVE_NO_MODULE
#define EVE_DEVICE       EVE_BT817
#define EVE_PANEL        EVE_NO_PANEL
#define EVE_DISPLAY_RES  EVE_WVGA
#define EVE_COPRO_METHOD EVE_COPRO_CMD_WRITE

/* Leave EVE_QSPI_ENABLE undefined for single-SPI bring-up. */
```

Use the actual hardware's values. The example chooses command-FIFO polling to avoid depending on INT# during first bring-up. The library also supports its configured command-buffer method on applicable devices. See [`EVE_config.h`][config-h] and the method definitions in [`EVE_defs.h`][defs-h].

For an application-local configuration, copy the **current** `include/EVE_config.h` and `include/EVE_defs.h`into the example platform directory and edit its existing entries. Retain its guard, compatible `EVE_defs.h` include and relevant feature settings. Do not append a second set of conflicting definitions. Put the local configuration directory before the repository include directory for **every** library and application source file, not just `main.c`. [Source: configuration override mechanism][config-h].

`EVE_QSPI_ENABLE` is a presence-based C preprocessor switch: `#define EVE_QSPI_ENABLE 0` still enables code guarded by `defined(EVE_QSPI_ENABLE)`. Leave it undefined for single SPI. CMake's `-DEVE_QSPI_ENABLE=OFF` is different: the shared recipe then omits the C definition. [Sources: configuration header][config-h], [shared CMake options][examples-cmake].

Do not select an integrated MCU/display module merely because its screen resolution matches. Module selections can also select touch or LCD-driver behaviour. Similarly, the current `EVE_settings.h` contains display-timing selection tables: a genuinely new timing requires deliberate support there, not an assumption that arbitrary `EVE_DISP_*` definitions will override those tables. [Source: derived configuration][settings-h].

**BT82x note:** A new platform macro is not automatically covered by the existing BT82x transport defaults. Configure the parameters described in **Section 9** before attempting a BT82x build.

**Checkpoint:** All translation units see the same device, display, transport method and feature selection.

## 6. Integrate the source files and build configuration

### Vendor IDE or manually maintained build

For an MCU-path build, add the following common library implementation files, along with the new MCU port:

```text
source/EVE_API.c
source/EVE_HAL.c
source/extensions/bt82x_patch.c
source/extensions/custom_touch_fw.c
source/extensions/lcd_panel_init.c
ports/eve_arch_newmcu/EVE_MCU_NEWMCU.c
```

The extension files contain conditional implementations; retain the appropriate current files rather than copying older generated application-note sources. The shared repository recipe includes these extensions as part of `eve_library`. [Source: common library source list][examples-cmake].

For the current simple example, also add:

```text
examples/simple/common/eve_example.c
examples/simple/common/eve_fonts.c
examples/simple/common/eve_images.c
examples/snippets/touch.c
examples/simple/newmcu/main/main.c
```

These are the current example sources, not the older `eve_helper.c` or `eve_calibrate.c` arrangement. Add the target's board-support files, SDK sources/libraries, startup code and linker configuration separately. [Source: simple-example source list][simple-cmake].

Supply these include locations as applicable:

```text
examples/simple/newmcu/          # First, if it contains EVE_config.h & EVE_defs.h
include/
examples/simple/common/
examples/snippets/
examples/simple/newmcu/board/
<vendor SDK and generated board include directories>
```

Define `PLATFORM_NEWMCU` consistently for the relevant targets and build configurations. Do not compile a donor MCU implementation alongside the new one. For a manually assembled MCU project, use `EVE_HAL.c`, not the Linux SPI-device HAL, and leave `USE_LINUX_SPI_DEV` undefined.

### Reuse the current CMake structure

The development branch separates three concerns:

| File | Current responsibility |
| --- | --- |
| [`examples/examples.cmake`][examples-cmake] | Derives the project name and repository paths, processes supported EVE options, calls `project()` and creates `eve_library`. |
| [`examples/simple/common.cmake`][simple-cmake] | Creates the example executable and the `eve_example` library containing the common simple-example sources. |
| Platform `CMakeLists.txt` and port `.cmake` file | Integrate the toolchain/SDK, add the selected MCU implementation and application entry point, and supply target-specific linking/output settings. |

Do not add another `add_library(eve_library ...)` or another executable with the generated name. Unlike the manual MCU-only source list, the shared recipe lists both HAL source files; their platform guards select the applicable implementation. The existing [Pico top-level file][pico-cmake] and [Pico port recipe][pico-port-cmake] show this arrangement.

The following is an integration template, not a complete vendor SDK project:

```cmake
# examples/simple/newmcu/CMakeLists.txt
cmake_minimum_required(VERSION 3.13)

# Supply the cross-compilation toolchain and any required pre-project SDK
# import before this include. examples.cmake itself calls project().
include(${CMAKE_CURRENT_SOURCE_DIR}/../../examples.cmake)

# Retain an explicit top-level project() call, as in the existing example.
project(${EXAMPLE_PROJECT_NAME} C CXX)

# Perform any SDK initialisation required after compiler detection here.

include(${CMAKE_CURRENT_SOURCE_DIR}/../common.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/../../../ports/eve_arch_newmcu/newmcu.cmake)

target_sources(${EXAMPLE_PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/main/main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/board/board_support.c
)

target_link_libraries(${EXAMPLE_PROJECT_NAME}
    eve_example
    eve_library
)

# Add vendor startup objects, SDK dependencies, the linker script and
# binary/flash output rules required by this specific MCU toolchain.
```

```cmake
# ports/eve_arch_newmcu/newmcu.cmake
add_compile_definitions(PLATFORM_NEWMCU)

target_sources(eve_library PRIVATE
    ${API_DIRECTORY}/ports/eve_arch_newmcu/EVE_MCU_NEWMCU.c
)

target_include_directories(eve_library PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/board
)

# Add the SDK include directories and dependencies needed to compile
# the MCU implementation. Match the vendor SDK's target model.
```

The directory layout matters because the shared recipe derives paths from the example platform directory. Follow the SDK's required initialisation order; a toolchain must not be introduced only after the first `project()` has already selected the compiler. [Sources: shared recipe][examples-cmake], [Pico integration][pico-cmake], [CMake source addition][cmake-target-sources].

After supplying the MCU-specific SDK integration, an illustrative command-line build is:

```sh
cmake -S examples/simple/newmcu -B build/simple-newmcu \
  -DCMAKE_TOOLCHAIN_FILE=/absolute/path/to/toolchain.cmake \
  -DEVE_DEVICE=EVE_BT817 \
  -DEVE_DISPLAY_RES=EVE_WVGA \
  -DEVE_COPRO_METHOD=EVE_COPRO_CMD_WRITE \
cmake --build build/simple-newmcu
```

Replace the toolchain and display selections with the actual ones. For a module-based build, use the appropriate `EVE_MODULE` instead of competing device/panel/resolution options.

**Current CMake detail:** When selecting device and resolution separately, omit the `EVE_MODULE` cache argument. Even the literal value `EVE_NO_MODULE` is a non-empty string that takes the shared recipe's module branch and prevents its device/panel/resolution arguments from being forwarded. Clear an old cached module selection or use a fresh build directory when changing selection style. Also, arbitrary `cmake -DNAME=value` arguments are not automatically C preprocessor definitions; unsupported options need explicit compile definitions or a suitable local configuration. [Source: option forwarding][examples-cmake].

**Checkpoint:** The new target links with exactly one MCU implementation and the intended effective configuration.

## 7. Connect the application entry point

### Preserve the library's initialisation sequence

The current simple example calls `EVE_Init()` itself. The relevant sequence is:

```text
main / vendor application task
  -> board clocks, time base and other required early SDK setup
  -> eve_example()
       -> EVE_Init()
            -> HAL_EVE_Init()
                 -> lcd_driver_init()       [only with EVE_LCD_INIT]
                 -> MCU_Init()
                 -> EVE reset and device-specific boot sequence
                 -> MCU_Setup()
                 -> SPI-width / interrupt configuration, when enabled
            -> remaining EVE display/touch/extension configuration
       -> eve_calibrate()
       -> eve_init_fonts()
       -> eve_load_images()
       -> eve_display()
```

This sequence follows the [simple example][simple-c], [EVE initialisation implementation][api-c] and [HAL initialisation][hal-c]. Do not add an unconditional `MCU_Init()` in `main()` simply because an older example placed it there: the current normal startup path already calls it.

**LCD-driver ordering matters:** With `EVE_LCD_INIT`, the current HAL calls `lcd_driver_init()` **before** `MCU_Init()`. Any clocks, GPIO access or time base required by that panel driver must therefore be prepared by early board setup or the driver itself. The filename is `lcd_panel_init.c`; its callable function is `lcd_driver_init()`. [Sources: extension declaration][lcd-h], [HAL startup][hal-c].

A minimal application entry point can follow this shape:

```c
#include <stdint.h>
#include "board_support.h"
#include "eve_example.h"  /* Common example interface and related declarations. */

int main(void)
{
    /* Project-specific helper: establish early SDK, clock and time-base needs.
     * EVE interface setup itself remains in MCU_Init().
     */
    if (board_initialise() != 0)
    {
        for (;;) { /* Report or retain the board-initialisation fault. */ }
    }

    eve_example();

    /* The normal demo runs continuously. Reaching here should be diagnosed. */
    for (;;) { /* Inspect earlier initialisation/calibration diagnostics. */ }
}
```

`board_initialise()` is an illustrative application helper, not a required library API. Preserve the vendor's entry-point signature or task model when it differs from this bare-metal example. Ensure there is only one `main()`.

### Supply the calibration-storage hooks

The simple example uses the calibration functions in [`examples/snippets/touch.c`][touch-c]. Its storage callbacks are declared in [`touch.h`][touch-h]. They are application/platform hooks, not additional functions to invent in `MCU.h`, and they are unrelated to the Linux `Platform.h` transport interface.

For initial testing without persistent storage, place the following in `main.c` or a separate application support file that includes `touch.h`:

```c
int8_t platform_calib_init(void)
{
    return 1; /* Persistent calibration storage is not implemented. */
}

int8_t platform_calib_read(struct touchscreen_calibration *calib)
{
    (void)calib;
    return -1;
}

int8_t platform_calib_write(struct touchscreen_calibration *calib)
{
    (void)calib;
    return -1;
}
```

Provide all three callbacks. In the current generic calibration path, the write callback can still be reached after interactive calibration even when the storage initialisation callback reports unavailable storage. These stubs permit recalibration without falsely reporting that uninitialised calibration data is valid. Some selected panel/module configurations supply predefined transforms and bypass interactive calibration. [Sources: callback contract][touch-h], [calibration control flow][touch-c].

### Make failures observable

Use a debugger, GPIO milestones or a working serial channel before relying on library logs. `EVE_DEBUG_LEVEL` controls verbosity only where an output mapping exists; the current [`EVE_debug.h`][debug-h] does not automatically know how to print for an arbitrary new platform macro. Add an appropriate mapping or use independent board diagnostics. Do not select an unrelated platform macro to borrow its logger.

**Checkpoint:** Execution reaches the EVE startup path once, and failures can be located without guessing from a blank screen.

## 8. Bring up the display in stages

### Stage A: Confirm EVE boot

Use breakpoints or diagnostic markers around `MCU_Init()`, the HAL's device-identification polling and `MCU_Setup()`. If the port returns from `MCU_Init()` but never reaches `MCU_Setup()`, inspect reset, power, SPI framing and device selection before changing display-list code.

The current HAL expects an identification value of `0x7C`. It uses the selected generation's register definitions and different boot/read sequences for EVE API 1-4 and BT82x. Do not hard-code the FT81x register address or its read framing into a supposedly device-independent MCU port. [Source: generation-specific boot implementation][hal-c].

**Important limitation:** The current HAL contains boot polling/retry loops without an overall finite deadline. Checking the return value of `EVE_Init()` is necessary but does not guarantee that a missing or unresponsive device will make it return. A working `MCU_Time_ms()` alone does not bound those loops. For production fault handling, review boot timeouts and recovery explicitly rather than assuming the example already supplies them. [Source: current boot loops][hal-c].

### Stage B: Display a message without touch or asset loading

Temporarily call a minimal diagnostic function instead of `eve_example()`. This removes calibration, image downloads and custom-font data from the initial test. Call it once after early board setup; do not call both initialisation paths in succession.

```c
#include <EVE.h>

int eve_bringup_display(void)
{
    if (EVE_Init() != 0)
    {
        return -1;
    }

    EVE_LIB_BeginCoProList();
    EVE_CMD_DLSTART();
    EVE_CLEAR_COLOR_RGB(0, 0, 0);
    EVE_CLEAR(1, 1, 1);
    EVE_COLOR_RGB(255, 255, 255);
    EVE_CMD_TEXT(EVE_DISP_WIDTH / 2, EVE_DISP_HEIGHT / 2,
                 28, EVE_OPT_CENTERX | EVE_OPT_CENTERY,
                 "EVE interface OK");
    EVE_DISPLAY();
    EVE_CMD_SWAP();
    EVE_LIB_EndCoProList();

    /* This limits command-completion waiting, not EVE_Init() above. */
    return EVE_LIB_AwaitCoProEmptyTimeout(1000u);
}
```

This uses the current public API and the same list-submission pattern as the simple example. `EVE_LIB_EndCoProList()` ends submission; explicitly wait for completion afterwards. Do not assume `EVE_LIB_BeginCoProList()` waits for a previous list to finish. A zero timeout selects an unbounded wait, so use a non-zero diagnostic timeout deliberately. [Sources: public API][eve-h], [list implementations][api-c], [simple example][simple-c].

A successful message establishes more than a successful compile: it exercises boot, register access, co-processor commands and visible display output. It does not yet validate every transfer size, touch, persistent storage or high-speed operation.

### Stage C: Check memory transfers

Select a confirmed unused, suitably aligned region of `RAM_G` and perform a small write/read-back test outside an active co-processor list. Do not blindly use address zero or a fixed donor-example offset: display assets and fonts may already occupy memory.

Use distinctive patterns to reveal missing bytes and byte-order errors, then increase the tested length across the driver's internal transfer boundaries. Use accesses supported by the selected EVE generation; do not assume the older byte/halfword register tests apply unchanged to BT82x. The [HAL memory operations][hal-c] and [BT82x patch extension][bt82x-c] are the references for the active implementation.

Treat this as a separate diagnostic, not an alteration to the application's asset layout. Restore any temporarily used memory before resuming an example that owns it.

### Stage D: Run the complete simple example

Restore the normal call to `eve_example()`. Complete calibration when requested, then verify the logo, font/image rendering and touch-sensitive counter. The current example waits for a touch tag before updating the counter; an unchanged number without a touch is not, by itself, evidence of a failed SPI port. [Source: current example loop][simple-c].

If the minimal message works but the full example fails, concentrate on the added paths: calibration and touch configuration, asset loading, larger transfers, available memory and co-processor completion. Do not immediately rewrite the already working boot sequence.

**Checkpoint:** Cold boot, visible output, memory transfers and the complete simple example work at the conservative SPI speed.

## 9. Add optional features

Introduce one feature at a time and repeat the staged tests after each change.

### Higher SPI speed, DMA and shared-bus operation

Increase the SPI speed through `MCU_Setup()` only after basic operation is stable. Measure the actual clock and retest long transfers, read-back and repeated cold starts. Choose the operating rate from the EVE device, host peripheral and physical connection limits, not the largest value accepted by an SDK call.

DMA or queued transfers must preserve the original ordering and buffer-lifetime guarantees. A scalar wrapper can pass the address of a local variable; asynchronous code must not continue using that stack address after the wrapper returns. Either keep the operation synchronous at the interface boundary or copy data into storage owned by the port. Verify cache maintenance and alignment where the MCU requires them.

In an RTOS or on a shared bus, protect the **whole transaction**, including CS# assertion, address and data transfers, and CS# release. A mutex around each individual byte transfer is insufficient. Start with one owner of the EVE API; re-entrant access and ISR-driven EVE commands require a separate concurrency design. These are design requirements for the proposed port, not guarantees supplied by a generic vendor SPI driver.

### INT# and interrupt-based co-processor completion

Bring up polling first. To add the INT# method, wire the signal, configure it as an input and select `EVE_COPRO_INT`. The derived configuration enables the library's interrupt-related path. An MCU interrupt service routine is not required simply to use this completion mechanism: the current HAL samples `MCU_Int()` while waiting. [Sources: method selection][settings-h], [HAL completion wait][hal-c].

`MCU_Int()` reports the **physical level of the active-low INT# input**, not a Boolean "interrupt pending" value. For a supported input, use the following return values:

| INT# pin level | Interrupt state | `MCU_Int()` return value |
| --- | --- | --- |
| Low | Asserted | `0` |
| High | Deasserted | Non-zero, normally `1` |

This contract is documented in [`MCU.h`][mcu-h] and [`HAL.h`][hal-h]. `HAL_WaitCmdFifoEmpty()` waits while `MCU_Int()` is non-zero, then checks the interrupt flags for `EVE_INT_CMDEMPTY`. [Source: HAL completion logic][hal-c].

For example, using a project-specific GPIO helper that returns `0` for a low input and `1` for a high input:

```c
int MCU_Int(void)
{
    /* Preserve the physical INT# level: low means asserted. */
    return board_eve_int_level();
}
```

`board_eve_int_level()` is an illustrative helper to implement with the target SDK and declare in the project's board-support header; it is not an EVE-MCU-Dev function. Do not invert a raw pin-level reading. If an SDK helper instead reports a logical "interrupt asserted" state, adapt its result to the pin-level convention above.

For a polling-only implementation without a usable interrupt pin, an explicit unsupported result can be combined with a build-time rejection of the interrupt method:

```c
#if defined(EVE_USE_INTERRUPT_METHOD)
#error "This port has no usable EVE INT# input. Select a polling method."
#endif

int MCU_Int(void)
{
    return -1; /* Unsupported; this must not enter the interrupt wait loop. */
}
```

The public interrupt-input API documents `-1` as unsupported; this is not a valid sampled pin level or an automatic fallback to polling. Retain the build-time rejection when using this stub: the HAL's non-zero test would otherwise keep waiting until a configured timeout, or indefinitely when no timeout is supplied. [Sources: public interrupt API][eve-h], [HAL completion logic][hal-c].

For a wired implementation, use the GPIO implementation instead of the stub and remove the rejection once tested. Before enabling interrupt-based completion, observe both the idle and asserted INT# states and verify the corresponding return values. Then repeat a co-processor completion test using a non-zero diagnostic timeout.

`MCU_Int()` should sample the host input, not perform EVE SPI transactions or clear EVE interrupt flags. Close the submitted co-processor list before waiting. Avoid arbitrary diagnostic reads of the clear-on-read EVE interrupt flags when interrupt management is in use; use the library's intended handling so pending events are not consumed unexpectedly. [Sources: HAL completion logic][hal-c], [public interrupt-management contract][eve-h].

### Quad SPI

Enable `EVE_QSPI_ENABLE` only for a supported EVE device and a port that implements the required transfer behaviour and wiring. Implement `MCU_SetSPIMode(uint8_t mode)` behind the same feature guard.

The HAL configures EVE's interface width and then requests the corresponding host-side change. `MCU_SetSPIMode()` should configure the MCU peripheral/pin directions; it should not issue EVE register writes or recursively call the HAL's mode-setting operation. Reject unsupported modes with a failure result. Check read direction changes and transaction framing rather than assuming a peripheral's serial-flash API directly matches EVE. [Sources: host-width interface][mcu-h], [HAL mode sequencing][hal-c], [FT9XX Quad SPI implementation][ft9xx-c].

### BT82x transport parameters

The current `MCU.h` supplies BT82x defaults through a list of recognised platform macros. A new `PLATFORM_NEWMCU` is not automatically included. Supply suitable `EVE_SPI_MAX_TRANSFER` and `EVE_SPI_TIMEOUT` values in a configuration visible to the HAL and port, such as the application-local `EVE_config.h` or consistent compile definitions. Defining them only inside the new MCU `.c` file is insufficient. [Source: BT82x defaults][mcu-h].

For an initial implementation that remains at **1 MHz single SPI**, the following values are a starting example, not a setting to retain without checking other speeds:

```c
#define EVE_SPI_MAX_TRANSFER 4
#define EVE_SPI_TIMEOUT      8
```

`EVE_SPI_TIMEOUT` is a **byte count in the BT82x read protocol**, not milliseconds. Recalculate/validate the read window against the actual SPI clock and interface width, including the speed selected by `MCU_Setup()`. `EVE_SPI_MAX_TRANSFER` controls the relevant HAL read chunking; it is not a promise that every call into the MCU driver will be that small. The current boot path, for example, requests a 128-byte block read. [Sources: parameter definitions][mcu-h], [BT82x read and boot operations][hal-c].

Keep these parameters distinct from `EVE_HAL_CHUNK_SIZE`, which is a common HAL chunking setting. Review stack use and SDK transfer limits, and use the current names rather than introducing deprecated `MCU_SPI_TRANSFER` or `MCU_SPI_TIMEOUT` definitions. [Sources: HAL configuration][hal-h], [MCU compatibility definitions][mcu-h].

Also verify the actual BT82x memory/display configuration, including `EVE_RAM_G_CONFIG_SIZE`, and retain the applicable patch extension in the build. Do not port the NXP note's older EVE boot protocol into the new MCU implementation. [Sources: device configuration][config-h], [BT82x extension][bt82x-c].

### LCD panel initialisation and custom touch support

Some modules need panel-controller programming in addition to EVE initialisation. With `EVE_LCD_INIT`, the extension's `lcd_driver_init()` supplies this step. The current file includes a specific panel implementation, not a universal driver for arbitrary controllers. A different target may need different SDK/GPIO support and a different panel command sequence. [Sources: LCD extension][lcd-c], [LCD extension declaration][lcd-h].

A panel interface may share the EVE SPI bus with a separate chip-select signal, use another peripheral, or use GPIO bit-banging. Keep both devices deselected when changing a shared peripheral's settings, and restore the EVE transport configuration before using it. Remember the pre-`MCU_Init()` ordering described in **Section 7**.

Custom touch firmware selection belongs to the supported device/module configuration and extension path, not the generic SPI primitives. Keep the feature guards and selected hardware consistent. Do not disable a module-required panel or touch step merely to obtain a successful build. [Sources: configuration][config-h], [derived module settings][settings-h].

### Persistent calibration storage

After touch is working, replace the temporary callbacks with an implementation appropriate to the MCU's non-volatile memory. Store and retrieve the declared `struct touchscreen_calibration`; the common touch code manages the calibration marker and transform values. Report failure when no valid stored record can be read. [Sources: stored record and callback declarations][touch-h], [calibration ownership][touch-c].

Reserve storage through the target's memory/linker arrangement. Do not reuse another MCU's hard-coded flash offset. Check erase/program granularity, alignment, endurance, power-loss behaviour and any restrictions on executing from flash while it is being written. Keep this support in the application/platform storage layer rather than adding flash assumptions to the common EVE library.

## 10. Troubleshooting

Use the failed checkpoint to narrow the investigation. The checks below are diagnostic suggestions; several different faults can produce the same visible symptom.

| Symptom | Checks |
| --- | --- |
| Missing `MCU_*` symbols | Is the new port source compiled? Does its platform guard match the build definition? Do signatures match the current header? |
| Duplicate symbols | Look for two MCU implementations, a second `main()`, or common sources added both directly and through a library target. |
| Missing calibration callbacks | Add all three `platform_calib_*` functions with the `touch.h` signatures. |
| Startup never reaches `MCU_Setup()` | Check the HAL boot polling point, supplies, PD#, SPI mode, actual pin mapping, `CS#` framing and the selected device. Boot does not have a universal deadline. |
| Reads are consistently all zeros or all ones | Inspect MISO direction and continuity, device selection/power and generated read clocks. Check the applicable read protocol rather than assuming one dummy-byte scheme for every generation. |
| Data is shifted or corrupted | Look for `CS#` pulses between phases, extra/missing dummy bytes, double byte-swapping, RX overrun or count truncation. |
| Small transfers work; image loading fails | Test driver length boundaries, buffering, stack use, source-buffer lifetime and memory allocation. Lower the clock to separate signal-integrity faults from software faults. |
| Registers respond but the screen is blank | Verify panel/module selection, output timings, backlight control, panel power and any required LCD-driver initialisation. |
| Calibration appears to hang | Check touch-controller configuration, required firmware, orientation and actual touch input. Test the no-touch diagnostic screen separately. |
| Counter does not change | The current simple example waits for touch. Confirm the expected tag before treating the static counter as a transport failure. |
| Polling works; `INT#` completion hangs | Check wiring, input configuration and interrupt selection. Verify `MCU_Int()` returns `0` for asserted/low and non-zero for deasserted/high, and that an unsupported-input stub is not being used. |
| BT82x build lacks transport macros, or reads fail after a speed change | Supply new-platform defaults, then validate the read window against actual clock/width and driver block-size handling. |
| No debug output | Verify board output independently and add the new platform's mapping in `EVE_debug.h`; verbosity alone does not create a backend. |

Implementation references for these checks: [MCU interface][mcu-h], [HAL][hal-c], [simple example][simple-c], [calibration][touch-c], [debug mapping][debug-h].

## 11. Validate and document the completed port

Before treating the port as complete, verify the following on the actual target:

- [ ] Clean debug and release builds use the intended configuration and one MCU implementation.
- [ ] Cold power-up, MCU reset, and repeated initialisation/shutdown where supported produce predictable pin states and display behaviour.
- [ ] The basic screen, read-back tests and full simple example pass at both the initial and selected operating SPI rates.
- [ ] Transfer lengths around driver limits are handled without data loss or unintended CS# transitions; zero-length operations are harmless.
- [ ] Delays and the millisecond clock remain correct after clock changes, and the application's timeout/recovery policy has been exercised.
- [ ] Touch and any optional storage, INT#, Quad SPI, LCD-driver or BT82x-specific paths are tested individually, not merely compiled.
- [ ] When `INT#` is supported, `MCU_Int()` follows the documented low/high return values and co-processor completion succeeds; unsupported-input builds reject the interrupt method.
- [ ] Shared-bus or RTOS access cannot interleave EVE transactions, and any DMA/buffering honours buffer lifetimes and completion ordering.
- [ ] Disconnected-device and interrupted-transfer behaviour is observable and does not falsely report successful operation.

Document the port's wiring, electrical assumptions, startup requirements, SDK/toolchain versions, build/flash commands, tested EVE device/module, SPI speeds and unsupported features in its README. Include the repository commit used for validation and the calibration-storage arrangement, where implemented. Retain relevant notices when adapting existing source files.

A useful completion criterion is: **another developer can build the example, connect the documented hardware and reproduce the basic display and touch tests without editing the common EVE API or HAL to compensate for MCU-specific transport behaviour.** Any necessary shared-library change should instead be identified explicitly, with its reason and separate validation.

## Source reference index

The primary references are [BRT_AN_062][an062] for the staged porting workflow and the [development branch][development] for the active implementation. Start with [MCU.h][mcu-h], [EVE_HAL.c][hal-c], [EVE_config.h][config-h] and [EVE_settings.h][settings-h] when checking the interface against a newer checkout.

For build integration, use [examples/examples.cmake][examples-cmake], [simple/common.cmake][simple-cmake] and the [Pico platform example][pico-cmake]. For application startup and storage, use [simple/common/eve_example.c][simple-c], [touch.h][touch-h] and [touch.c][touch-c]. Source-specific links are also provided beside the relevant instructions throughout this guide.

[development]: https://github.com/Bridgetek/Eve-MCU-Dev/tree/development
[an062]: https://brtchip.com/wp-content/uploads/2024/04/BRT_AN_062_Porting_BRT_AN_025_to_NXP_MCU.pdf
[readme]: https://github.com/Bridgetek/Eve-MCU-Dev/blob/development/README.md
[ports-readme]: https://github.com/Bridgetek/Eve-MCU-Dev/blob/development/ports/README.md
[simple-readme]: https://github.com/Bridgetek/Eve-MCU-Dev/blob/development/examples/simple/README.md
[mcu-h]: https://github.com/Bridgetek/Eve-MCU-Dev/blob/development/include/MCU.h
[hal-h]: https://github.com/Bridgetek/Eve-MCU-Dev/blob/development/include/HAL.h
[hal-c]: https://github.com/Bridgetek/Eve-MCU-Dev/blob/development/source/EVE_HAL.c
[eve-h]: https://github.com/Bridgetek/Eve-MCU-Dev/blob/development/include/EVE.h
[api-c]: https://github.com/Bridgetek/Eve-MCU-Dev/blob/development/source/EVE_API.c
[config-h]: https://github.com/Bridgetek/Eve-MCU-Dev/blob/development/include/EVE_config.h
[defs-h]: https://github.com/Bridgetek/Eve-MCU-Dev/blob/development/include/EVE_defs.h
[settings-h]: https://github.com/Bridgetek/Eve-MCU-Dev/blob/development/include/EVE_settings.h
[debug-h]: https://github.com/Bridgetek/Eve-MCU-Dev/blob/development/include/EVE_debug.h
[rp2040-c]: https://github.com/Bridgetek/Eve-MCU-Dev/blob/development/ports/eve_arch_rpi/EVE_MCU_RP2040.c
[stm32-spi-c]: https://github.com/Bridgetek/Eve-MCU-Dev/blob/development/ports/eve_arch_stm32/EVE_MCU_STM32CUBE_SPI.c
[stm32-qspi-c]: https://github.com/Bridgetek/Eve-MCU-Dev/blob/development/ports/eve_arch_stm32/EVE_MCU_STM32CUBE_QUADSPI.c
[ft9xx-c]: https://github.com/Bridgetek/Eve-MCU-Dev/blob/development/ports/eve_arch_ft9xx/EVE_MCU_FT9XX.c
[examples-cmake]: https://github.com/Bridgetek/Eve-MCU-Dev/blob/development/examples/examples.cmake
[simple-cmake]: https://github.com/Bridgetek/Eve-MCU-Dev/blob/development/examples/simple/common.cmake
[pico-cmake]: https://github.com/Bridgetek/Eve-MCU-Dev/blob/development/examples/simple/pico/CMakeLists.txt
[pico-port-cmake]: https://github.com/Bridgetek/Eve-MCU-Dev/blob/development/ports/eve_arch_rpi/pico.cmake
[simple-c]: https://github.com/Bridgetek/Eve-MCU-Dev/blob/development/examples/simple/common/eve_example.c
[touch-h]: https://github.com/Bridgetek/Eve-MCU-Dev/blob/development/examples/snippets/touch.h
[touch-c]: https://github.com/Bridgetek/Eve-MCU-Dev/blob/development/examples/snippets/touch.c
[lcd-h]: https://github.com/Bridgetek/Eve-MCU-Dev/blob/development/include/extensions/lcd_panel_init.h
[lcd-c]: https://github.com/Bridgetek/Eve-MCU-Dev/blob/development/source/extensions/lcd_panel_init.c
[bt82x-c]: https://github.com/Bridgetek/Eve-MCU-Dev/blob/development/source/extensions/bt82x_patch.c
[cmake-target-sources]: https://cmake.org/cmake/help/latest/command/target_sources.html
