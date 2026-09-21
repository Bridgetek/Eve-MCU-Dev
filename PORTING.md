# EVE-MCU-Dev Porting Guide

This porting guide describes the steps required to add support for a new MCU or host platform to the `EVE-MCU-Dev` library.

## Introduction

The objective of an MCU port is to connect the common EVE-MCU-Dev library to the target's SPI, GPIO and timing facilities, while keeping EVE commands and register handling in the existing library layers. Beginning with a basic, blocking, single-SPI implementation and a known display configuration. Adding performance improvements and optional features only after communication and display output have been verified.

This guide adapts the staged approach in [BRT_AN_062: Porting BRT_AN_025 to NXP MCU][an062]: establish the development environment, test the low-level interface, integrate the common library, and run an example. Function names, source files and build instructions below are based on the current repository rather than the older version of the library utilised in the NXP project.

**Note:** The [NXP Port][nxp-c] is available as a supported platform in EVE-MCU-Dev and can be referenced during the porting process.

### Scope

The instructions primarily cover MCU ports implementing [`include/MCU.h`][mcu-h] and using [`source/EVE_HAL.c`][hal-c]. Linux SPI-device based ports will utilise `Platform.h` and `EVE_HAL_Linux.c` instead and are not a drop-in variation of the MCU implementation described here, though the described porting process remains the same. See the [repository overview][readme] for the wider EVE-MCU-Dev library architecture.

This guide does not replace the target MCU's SDK documentation, the selected EVE device's documentation, or the display module's schematic. Adding an unsupported display panel, touch controller or EVE device is separate from adding MCU support to the library. The templates are starting points: SDK-specific code must be supplied and validated on the target hardware.

## Contents

* [Introduction](#introduction)
  * [Scope](#scope)
* [1. Establish the starting point](#1-establish-the-starting-point)
  * [Obtain a consistent checkout](#obtain-a-consistent-checkout)
  * [Start from a working board project](#start-from-a-working-board-project)
* [2. Define the port and hardware interface](#2-define-the-port-and-hardware-interface)
  * [Keep the implementation boundaries clear](#keep-the-implementation-boundaries-clear)
  * [Establish the connections](#establish-the-connections)
  * [Independent chip-select control](#independent-chip-select-control)
* [3. Prove the MCU SPI and GPIO operations](#3-prove-the-mcu-spi-and-gpio-operations)
* [4. Implement the MCU interface](#4-implement-the-mcu-interface)
  * [Required function groups](#required-function-groups)
  * [Initialisation, setup and shutdown](#initialisation-setup-and-shutdown)
  * [Chip-select and power-down control](#chip-select-and-power-down-control)
  * [Blocking transfers and chip-select ownership](#blocking-transfers-and-chip-select-ownership)
  * [Scalar transfers and byte order](#scalar-transfers-and-byte-order)
  * [Timing](#timing)
  * [Interrupt input](#interrupt-input)
  * [Other implementation responsibilities](#other-implementation-responsibilities)
* [5. Select the EVE device and display configuration](#5-select-the-eve-device-and-display-configuration)
* [6. Integrate the source files and build configuration](#6-integrate-the-source-files-and-build-configuration)
  * [Vendor IDE or manually maintained build](#vendor-ide-or-manually-maintained-build)
  * [Reuse the current CMake structure](#reuse-the-current-cmake-structure)
* [7. Connect the application entry point](#7-connect-the-application-entry-point)
  * [Preserve the library's initialisation sequence](#preserve-the-librarys-initialisation-sequence)
  * [Supply the calibration-storage hooks](#supply-the-calibration-storage-hooks)
  * [Make failures observable](#make-failures-observable)
* [8. Bring up the display in stages](#8-bring-up-the-display-in-stages)
  * [Stage A: Confirm EVE boot](#stage-a-confirm-eve-boot)
  * [Stage B: Display a message without touch or asset loading](#stage-b-display-a-message-without-touch-or-asset-loading)
  * [Stage C: Check memory transfers](#stage-c-check-memory-transfers)
  * [Stage D: Run the complete simple example](#stage-d-run-the-complete-simple-example)
* [9. Add optional features](#9-add-optional-features)
  * [Increase SPI speed, DMA and shared-bus operation](#increase-spi-speed-dma-and-shared-bus-operation)
    * [Increase SPI speed](#increase-spi-speed)
    * [DMA and queued transfers](#dma-and-queued-transfers)
    * [Shared SPI buses and RTOS operation](#shared-spi-buses-and-rtos-operation)
  * [Quad SPI](#quad-spi)
  * [Persistent calibration storage](#persistent-calibration-storage)
  * [BT82x transport parameters](#bt82x-transport-parameters)
  * [Custom touch support](#custom-touch-support)
  * [LCD panel initialisation](#lcd-panel-initialisation)
  * [INT# and interrupt-based co-processor completion](#int-and-interrupt-based-co-processor-completion)
  * [Advanced chip-select implementations](#advanced-chip-select-implementations)
* [10. Troubleshooting](#10-troubleshooting)
* [11. Validate and document the completed port](#11-validate-and-document-the-completed-port)
* [Source reference index](#source-reference-index)


## 1. Establish the starting point

### Obtain a consistent checkout

Work from one revision of the EVE-MCU-Dev library. Do not combine an older `MCU.h`, application-note source file and a newer HAL implementation from the current version of the library.

```sh
git clone --branch development --recurse-submodules https://github.com/Bridgetek/Eve-MCU-Dev.git
cd Eve-MCU-Dev
git rev-parse HEAD
```

For an existing checkout, update its submodules to the revisions recorded by that checkout:

```sh
git submodule update --init --recursive
```

**Note:** EVE-MCU-Dev currently utilises submodules for USB based host implementations with external driver dependencies; [FT4222][4222-c], [MPSSE][mpsse-c]. A submodule is also utilised for the [EVE_Emulator][emulator-c] port to obtain the required dependencies.

Record the repository commit, compiler, SDK, IDE or CMake version, board revision and display module. This makes later failures reproducible. External dependencies are port-dependent; inspect the [ports documentation][ports-readme] rather than assuming every MCU needs the same host libraries.

### Start from a working board project

Install the IDE for the chosen MCU, connect the MCU development baord to the host PC, and create or import a vendor-supported project for the target board. Download, build, and run a simple LED or serial-output test before introducing EVE-MCU-Dev.
Confirm that the debugger, system clock, GPIO configuration and a usable time base work before attempting to integrate the library.

Retain any necessary startup code, linker scripts, clock configuration and SDK initialisation from the example project. Replacing a generated `main.c` with a donor example from one of the other supported ports can accidentally remove these prerequisites.

Choose an existing port with a similar SDK or peripheral interface as a reference. The [RP2040 implementation][rp2040-c] demonstrates a direct blocking SPI implementation; the [STM32 SPI implementation][stm32-spi-c] demonstrates another SDK integration. Copy the organisation and required interface, not the donor's pins, clock frequencies or flash addresses. The current [simple-example README][simple-readme] marks completed build environments, these can be referenced for a  verified current build recipe.

**_Checkpoint:_** A standalone MCU development board project that runs reliably, and whose library revision is recorded.

## 2. Define the port and hardware interface

### Keep the implementation boundaries clear

The normal call path for MCU specific ports is:

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

This is a runtime/interface overview, not a header-include diagram. The MCU implementation supplies transport and host operations. It should not generate display lists to render items on EVE based screens, interpret EVE registers, or call back into the EVE API to perform a transfer. `MCU.h` includes `EVE_settings.h` for derived build-time configurations which may be required for the port; an ordinary MCU transport implementation does not need `EVE.h` or `HAL.h`.

**Sources:** [Library architecture][readme], [MCU interface][mcu-h].

A proposed layout for a new port within the library is:

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
      main/
        main.c
      board/
        board_support.c          # Illustrative board/SDK glue
        board_support.h
```

`newmcu`, `PLATFORM_NEWMCU` and `board_support.*` are placeholders introduced by this guide, not existing repository interfaces. Use a distinct platform macro consistently in the build and the new MCU port source file. **Do not** define another MCU's macro merely to make its code compile.

### Establish the connections

Create a pin-assignment table for the actual MCU development board/module before writing the port source code.

| EVE connection | MCU-side requirement |
| --- | --- |
| SCK | SPI master clock output. |
| MOSI / IO0 | SPI transmit connection; bidirectional data line when the selected multi-line interface requires it. |
| MISO / IO1 | SPI receive connection; bidirectional data line when required. |
| IO2 and IO3 | Additional data connections when Quad SPI is used. |
| CS# | Active-low chip select controlled independently of the SPI peripheral, normally using a separate GPIO output. **Do not use** automatic peripheral/SDK chip-select control for the conventional blocking SPI implementation. |
| PD# | Active-low power-down/reset control, normally a GPIO output. |
| INT# | **Optional** input for interrupt-based completion or application interrupt handling. Do not configure it as an MCU output. |
| Supply and ground | Correct module supply, adequate current capability and a common signal ground. |

Check the module's schematic and electrical limits. A module powered from **5V** does not necessarily accept **5V** on its SPI or control inputs. Generally EVE based display modules will utilise **3V3** signals on the SPI interface. Verify required pull-ups, level translation, power sequencing and the treatment of unused data pins against the selected hardware documentation. These are hardware-specific requirements, not values to copy from the NXP wiring diagram.

**Source:** [Hardware approach in BRT_AN_062][an062].

For the new MCU port implementation, choose SPI master mode, SPI Mode 0, 8-bit transfers and MSB-first bit order. A conservative initial clock such as 1 MHz is a useful starting point, as demonstrated by the [RP2040 port][rp2040-c]. Do not assume that its later operating speed is suitable for another board or EVE device.

### Independent chip-select control

**Due to the library structure, CS# must be controlled independently of the SPI peripheral's automatic chip-select mechanism. For the MCU SPI port, assign CS# to a separate GPIO output and control it through `MCU_CSlow()` and `MCU_CShigh()`.**

The library defines EVE transaction boundaries separately from the SPI read and write operations. **Do not use** automatic chip-select assertion, deassertion and pulse generation in the SPI peripheral or its driver. A pin which also supports an SPI chip-select alternate function may still be used, provided it is configured as a software-controlled GPIO rather than the peripheral's automatically controlled chip-select output.

A single EVE API transaction can contain several successive SPI function calls. CS# must remain asserted across these calls until the library explicitly ends the transaction. Releasing CS# after each byte, word or buffer would split the intended transaction. For example, the EVE API 1-4 implementation of `HAL_HostCmdWrite()` sends its command, parameter and final zero byte through three separate `MCU_SPIWrite8()` calls under one CS# assertion.

**Source:** [HAL host-command implementation][hal-c].

Using a buffer-based SDK SPI API is acceptable; direct register access and byte-at-a-time SDK calls are not requirements. The important condition is that the SPI API does not take ownership of the GPIO-controlled CS# line. Peripherals which cannot provide this separation need the transaction-aware adaptation described under [Advanced chip-select implementations](#advanced-chip-select-implementations), not an unmodified per-call SPI wrapper.

**_Checkpoint:_** Every signal has a documented pin, direction, voltage and initial state, and CS# is independently controlled.

## 3. Prove the MCU SPI and GPIO operations

Test the vendor SPI and GPIO routines before debugging the full EVE initialisation sequence. This preserves the useful separation between peripheral testing and library integration used in [BRT_AN_062][an062].

With EVE disconnected or kept deselected, transmit a recognisable test pattern and inspect SCK, MOSI and CS# pins with a logic analyser. Verify the selected pins, idle clock state, clock frequency, bit order and exact byte count. A loopback test can check receive handling when the wiring and disconnected peripheral arrangement permit it; remove the loopback before connecting EVE. **Do not** send arbitrary test patterns to a selected EVE device as though they were valid commands.

Then verify the intended transaction boundary: assert CS# through its GPIO, make several successive SDK transfers, and deassert CS# only after the final bit has left the peripheral. Test both consecutive writes and a write followed by a read. CS# must not pulse high between phases simply because they use separate function calls or the SDK splits a buffer internally. For example a 32 bit read/write consisting of 4 x 8 bit read operations should be contained within a single CS# pulse.

Also check that a read generates clock pulses, that unused received data from writes cannot accumulate into an RX overrun, and that the delay/time functions work at the final MCU system-clock setting.

**_Checkpoint:_** The MCU can clock bytes in both directions while preserving a manually controlled transaction boundary.

## 4. Implement the MCU interface

Use the declarations in the checkout's [`include/MCU.h`][mcu-h] as the interface checklist. The [BRT_AN_062][an062] application note's combined `MCU_SPIReadWrite8()` is not a replacement for the current public read and write functions. 

The examples below describe a conventional, blocking, GPIO-controlled SPI port. Names beginning with `board_` and `BOARD_EVE_` are illustrative project-specific helpers and pin identifiers, not EVE-MCU-Dev interfaces. Declare and implement them in the board-support files using the target SDK (if required). Include `MCU.h` and the board-support header (if required) in the MCU implementation.

### Required function groups

| Group | Functions to implement | Responsibility |
| --- | --- | --- |
| Lifecycle | `int MCU_Init(void)`, `int MCU_Setup(void)`, `int MCU_Deinit(void)` | Configure, adjust and release the host interface. Return `0` on success and `-1` on failure. |
| Control pins | `MCU_CSlow()`, `MCU_CShigh()`, `MCU_PDlow()`, `MCU_PDhigh()` | Control CS# independently of the SPI peripheral and PD#. Apply the named physical levels. |
| Interrupt input | `int MCU_Int(void)` | Return `0` for EVE INT# pin assertion (logic low) and non-zero for deassertion (logic high). Explicitly reject unsupported interrupt use; see [Section 9](#int-and-interrupt-based-co-processor-completion). |
| Block transfers | `MCU_SPIWrite(const uint8_t *, uint32_t)`, `MCU_SPIRead(uint8_t *, uint32_t)` | Transfer the requested bytes without changing CS# or adding protocol framing. |
| Scalar transfers | `MCU_SPIWrite8()`, `MCU_SPIWrite16()`, `MCU_SPIWrite24()`, `MCU_SPIWrite32()`, `MCU_SPIRead8()`, `MCU_SPIRead16()`, `MCU_SPIRead32()` | Supply the fixed-size operations used by the HAL. |
| Timing | `MCU_Delay_20ms()`, `MCU_Delay_500ms()`, `uint32_t MCU_Time_ms(void)` | Supply minimum delays and an advancing millisecond count. |
| Host to wire byte order | `MCU_htobe16()`, `MCU_htobe32()`, `MCU_htole16()`, `MCU_htole32()` | Convert host values to the specified byte order. |
| Wire to host byte order | `MCU_be16toh()`, `MCU_be32toh()`, `MCU_le16toh()`, `MCU_le32toh()` | Convert the specified byte order to host values. |
| Optional interface width | `int MCU_SetSPIMode(uint8_t mode)` | Configure the host SPI width when `EVE_QSPI_ENABLE` is defined. |

There is no active `MCU_SPIRead24()` requirement. Implement the baseline interface rather than relying on the first selected example to exercise every function. Keep function names, parameter types and return types consistent with the current header. 

**Source:** [Current MCU.h declarations][mcu-h].

### Initialisation, setup and shutdown

| Function | Implementation requirements |
| --- | --- |
| `MCU_Init()` | Establish the SPI and GPIO resources needed for EVE. For initial single-SPI operation, configure SPI master mode 0, eight-bit transfers and MSB-first bit order. **Do not use** automatic hardware CS#. Set CS# and PD# inactive (logic high), configure a connected INT# as an input (if desired), and select a conservative SPI frequency such as 1 MHz. |
| `MCU_Setup()` | Apply MCU-side interface settings appropriate after EVE has booted, such as increasing the SPI clock. Preserve independent GPIO control of CS#. Returning `0` without changing anything is sufficient. |
| `MCU_Deinit()` | Complete outstanding activity, leave CS# inactive (logic high), apply the intended PD# shutdown state (logic low) and release resources owned by the port. Do not shut down or reset a shared peripheral without accounting for its other users. |

These functions return `0` on success and `-1` on failure. The [RP2040 implementation][rp2040-c] provides examples of initial configuration, post-boot speed adjustment and shutdown; the [MCU interface][mcu-h] defines the lifecycle entry points.

Where the MCU permits it, preload inactive output levels before enabling the GPIO output drivers to avoid unwanted CS# or PD# pulses. Configure the connected INT# signal as an input (if required); do not drive it high from the MCU as though it were an output. Select any required input pull-up from the module schematic and electrical requirements.

Preserve generated board setup. A vendor-generated project may already configure clocks, pin multiplexing, GPIOs or an SPI handle before the library starts. Define which setup belongs to the board project and which belongs to `MCU_Init()`; do not initialise the same resources twice without checking the SDK's requirements. For example, the [STM32 SPI implementation][stm32-spi-c] uses a handle supplied by the generated project.

Keep MCU-side configuration separate from EVE configuration. Do not duplicate EVE host commands, reset timing, register programming or display setup in these functions. The normal library startup already calls `MCU_Init()` and later `MCU_Setup()`; preserve that sequence. The optional LCD-panel extension is run before `MCU_Init()`, so its prerequisites may require earlier board setup, as described in [Section 7](#7-connect-the-application-entry-point).
    
**Source:** [HAL startup sequence][hal-c].

Check SDK return values. If initialisation fails, leave the interface in a safe state and release partially acquired resources where appropriate. Do not report successful initialisation after a failed peripheral configuration. Document the intended PD# shutdown state; existing reference ports assert PD# during deinitialisation.

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

### Chip-select and power-down control

For the conventional GPIO-controlled implementation, the control functions apply the named physical output levels:

| Function | Output action |
| --- | --- |
| `MCU_CSlow()` | Assert CS# by driving it low. |
| `MCU_CShigh()` | Deassert CS# by driving it high (after the final SPI bit has completed). |
| `MCU_PDlow()` | Assert PD# by driving it low. |
| `MCU_PDhigh()` | Release PD# by driving it high. |

The SPI transfer functions must not call the chip-select functions themselves. The library determines when a transaction starts and ends. The power-down functions change the output level only; leave EVE reset sequencing and its delays in the HAL. 

**Sources:** [HAL sequencing][hal-c], [RP2040 GPIO-controlled reference port][rp2040-c].

```c
/* board_gpio_write() applies a physical level: 0 = low, 1 = high.
 * Define BOARD_EVE_CS_PIN and BOARD_EVE_PD_PIN for the actual board.
 */
void MCU_CSlow(void)
{
    board_gpio_write(BOARD_EVE_CS_PIN, 0);
}

void MCU_CShigh(void)
{
    /* The blocking SPI routines must have completed before this call. */
    board_gpio_write(BOARD_EVE_CS_PIN, 1);
}

void MCU_PDlow(void)
{
    board_gpio_write(BOARD_EVE_PD_PIN, 0);
}

void MCU_PDhigh(void)
{
    board_gpio_write(BOARD_EVE_PD_PIN, 1);
}
```

Establish the GPIO directions, pin multiplexing and initial levels during board/MCU initialisation, not on every pin transition. A driver that only queues bytes or fills a transmit FIFO is not necessarily finished transmitting. Either make the SPI functions wait for actual completion, as assumed above, or provide the completion handling required by a deliberately designed advanced transport before CS# rises.

### Blocking transfers and chip-select ownership

The simplest implementation completes every transfer before returning. Neither `MCU_SPIWrite()` nor `MCU_SPIRead()` should assert or deassert CS#. The HAL surrounds related transfer calls with the control-pin operations.

**Sources:** [HAL transfer implementation][hal-c], [RP2040 transfers][rp2040-c].

| Function | Required behaviour for the basic blocking port |
| --- | --- |
| `MCU_SPIWrite(const uint8_t *data, uint32_t length)` | Transmit exactly `length` bytes in buffer order. Consume or discard received data as required by the peripheral. Do not change CS#, add an address or insert protocol bytes. |
| `MCU_SPIRead(uint8_t *data, uint32_t length)` | Generate the clocks needed to receive exactly `length` bytes. During ordinary single-SPI reads, transmit zero-valued bytes and store the received bytes in buffer order. Do not change CS# or add transaction framing. |

The MCU interface specifies zero-valued transmit data during reads and discarding received data during writes.

**Source:** [SPI transfer declarations][mcu-h].

The following adapter assumes a project-specific `board_spi_exchange8()` which transmits one byte, consumes the received byte and waits for completion. The helper must leave CS# unchanged and expose SDK errors through the port's chosen fault-handling mechanism.

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

The bytes transmitted by `MCU_SPIRead()` generate receive clocks; they are not additional EVE protocol dummy bytes. The HAL supplies the applicable address and read-protocol handling. Do not prepend another address, discard an extra first received byte or insert another dummy-byte phase in the conventional MCU transport. 

**Source:** [HAL read-address and read-data handling][hal-c].

A blocking SDK buffer transfer can replace the byte loop. If the SDK has a smaller count type or a maximum transfer size, split the request internally without changing CS# or truncating the `uint32_t` length. Zero-length requests perform no transfer; non-zero requests require a valid buffer of at least the requested size. Handle write-side receive data so it cannot accumulate into an RX overrun.

Do not return from the blocking interface while the peripheral still depends on the caller's buffer. A DMA-complete indication may need an additional peripheral-busy check before the final bit has left the pin; follow the target SDK's documented completion semantics. Verify both source-buffer lifetime and on-wire completion rather than assuming they are the same event.

The transfer functions do not return an error status. Decide how SDK failures are exposed through diagnostics and the application's fault-handling policy. A failed transfer must not be silently treated as a valid read simply because the interface is `void`.

A peripheral that cannot expose independent CS# control is not a direct substitute for this implementation. See [Advanced chip-select implementations](#advanced-chip-select-implementations) for the additional obligations of a buffered or hardware-managed transport.

### Scalar transfers and byte order

The suffixes in `MCU_SPIWrite16()`, `MCU_SPIWrite24()` and `MCU_SPIWrite32()` describe the number of data bits transferred by the library operation. They do not require 16-, 24- or 32-bit hardware frames. Keep the initial SPI peripheral configuration at eight bits and implement these operations through the block-transfer functions.

MSB-first SPI bit order is not the same as byte order within a multi-byte value. The HAL prepares the protocol representation; scalar helpers preserve the resulting object bytes rather than adding an unconditional swap.

**Sources:** [HAL address/data operations][hal-c], [RP2040 scalar helpers][rp2040-c].

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
    /*
     * The HAL has already prepared the wire representation.
     * Send the first three bytes of the object, not an independently
     * reconstructed three-byte numeric value.
     */
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

`MCU_SPIWrite24()` must preserve the HAL's prepared three-byte address representation. Do not simply shift out the numeric low 24 bits or apply another byte swap.

Implement all eight conversion functions with the correct behaviour for the target:

| Host representation | Host <-> little-endian | Host <-> big-endian |
| --- | --- | --- |
| Little-endian MCU | Leave the value unchanged. | Swap its bytes. |
| Big-endian MCU | Swap its bytes. | Leave the value unchanged. |

Use the compiler or SDK's supported byte-swap operations, or a verified portable implementation. Do not assume a donor port's no-op conversions apply to a different architecture. Treat block transfers as byte streams; do not reinterpret bitmap data or a command buffer as an array of values to swap.

The following are mock-transport test vectors derived from this convention. Capture the emitted bytes in a test helper; these are not standalone commands to send to a selected EVE device.

| Test operation | Expected transmitted bytes |
| --- | --- |
| `MCU_SPIWrite24(MCU_htobe32(0x81234500u))` | `81 23 45` |
| `MCU_SPIWrite32(MCU_htole32(0x12345678u))` | `78 56 34 12` |

Also test the read direction: receiving `78 56 34 12` into `MCU_SPIRead32()` and applying `MCU_le32toh()` should produce `0x12345678u` on either host byte order.

### Timing

Make `MCU_Delay_20ms()` and `MCU_Delay_500ms()` wait for at least their stated durations. Implement `MCU_Time_ms()` from a running time source, not a variable incremented only when the application calls a delay.

**Source:** [Timing interface][mcu-h].

```c
/* Project-specific helpers:
 * board_delay_ms() waits for at least the requested duration.
 * board_time_ms() returns an advancing uint32_t millisecond counter.
 */
void MCU_Delay_20ms(void)
{
    board_delay_ms(20u);
}

void MCU_Delay_500ms(void)
{
    board_delay_ms(500u);
}

uint32_t MCU_Time_ms(void)
{
    return board_time_ms();
}
```

The counter should advance monotonically, apart from natural `uint32_t` wraparound. Calculate elapsed intervals using unsigned subtraction. If using an RTOS tick count, convert ticks to milliseconds with the correct tick rate and a suitably wide intermediate value. On an MCU which cannot read its counter atomically, take a consistent snapshot using the SDK's supported method.

Ensure the time base is available during startup and remains correct after clock changes. An interrupt-driven SDK delay can stop progressing while interrupts are disabled, and a calibrated delay loop can become inaccurate after a frequency change. Test these conditions rather than increasing arbitrary delay constants. The [STM32 CUBE implementation][stm32-common-c] illustrates where timing functions can be kept separately from the SPI implementation.

### Interrupt input

`MCU_Int()` samples the MCU input connected to EVE INT#. Return `0` when the physical input is asserted (logic low) and non-zero when it is deasserted (logic high). Do not change CS#, start an SPI transfer, clear EVE interrupt flags or wait inside this function for INT# to become active. **Note** Pin implementation is optional and only required where `EVE_COPRO_METHOD` is set to `EVE_COPRO_INT`.

**Sources:** [MCU input contract][mcu-h], [RP2040 GPIO input example][rp2040-c].

The wired-input example and unsupported-input safeguards are provided in [Section 9](#int-and-interrupt-based-co-processor-completion). Use one implementation, not both. Check the actual returned low/high values before enabling interrupt-based co-processor completion.

### Other implementation responsibilities

| Interface or feature | What the new port must provide |
| --- | --- |
| `MCU_SetSPIMode(uint8_t mode)` | When Quad SPI is enabled via `EVE_QSPI_ENABLE`, configure the MCU-side interface for supported modes and return failure for unsupported requests. Leave EVE-side mode programming in the HAL. |
| `platform_calib_init()`, `platform_calib_read()`, `platform_calib_write()` | Application/platform storage callbacks when using the touch-calibration example `touch.c`. These are not additions to `MCU.h`; temporary stubs are shown in [Section 7](#7-connect-the-application-entry-point). |
| Debug output | A suitable output mapping for the new platform when library diagnostics are required, support can be added in `EVE_debug.h` for a new MCU platfrom. |
| LCD-panel initialisation | Board-specific support for the selected panel extension when enabled, including prerequisites needed before `MCU_Init()`. **Note**: Only needed for LCD panels which require initialisation, and is enabled via the `EVE_LCD_INIT` define. |

Follow the existing interfaces rather than adding these responsibilities to the generic SPI primitives.

**Sources:** [MCU interface][mcu-h], [touch calibration callbacks][touch-h], [debug mappings][debug-h], [LCD panel initialisation][lcd-h].

**_Checkpoint:_** All required MCU functions build without donor-platform dependencies, and their transfers, pin levels and timing have been checked independently. CS# remains under the library's transaction control rather than the SPI driver's per-call control.

## 5. Select the EVE device and display configuration

Start with a supported module or a known device/panel configuration. **Changing both the MCU transport and an unverified display timing configuration makes faults harder to isolate.**

The main selection values are `EVE_MODULE`, `EVE_DEVICE`, `EVE_PANEL`, `EVE_DISPLAY_RES` and `EVE_COPRO_METHOD`. A recognised module selection overrides the separate device, panel and resolution choices; a recognised panel selection determines the resolution. [`EVE_settings.h`][settings-h] derives the effective configuration from these values. Do not manually define derived API-selection macros to disguise a mismatched device.

For example, the following values describe a BT817/WVGA configuration, not a universal display selection:

```c
/* Illustrative values to apply to the corresponding configuration entries. */
#define EVE_MODULE       EVE_NO_MODULE
#define EVE_DEVICE       EVE_BT817
#define EVE_PANEL        EVE_NO_PANEL
#define EVE_DISPLAY_RES  EVE_WVGA
#define EVE_COPRO_METHOD EVE_COPRO_CMD_WRITE

/* Leave EVE_QSPI_ENABLE undefined for single-SPI bring-up. */
```

Use the actual hardware's values. The example chooses command-FIFO polling to avoid depending on EVE INT# during first bring-up. The library also supports its configured command-buffer method on applicable devices. See [`EVE_config.h`][config-h] and the method definitions in [`EVE_defs.h`][defs-h].

For an application-local configuration, copy the **current** `include/EVE_config.h` and `include/EVE_defs.h` into the example platform directory. Edit the configuration entries in `EVE_config.h`, and keep the copied `EVE_defs.h` consistent with the same library revision. Retain its guard, compatible `EVE_defs.h` include and relevant feature settings. Do not append a second set of conflicting definitions. Put the local configuration directory before the repository include directory for **every** library and application source file, not just `main.c`.

**Source:** [Configuration override mechanism][config-h].

`EVE_QSPI_ENABLE` is a presence-based C preprocessor switch: `#define EVE_QSPI_ENABLE 0` still enables code guarded by `defined(EVE_QSPI_ENABLE)`. Leave it undefined for single SPI. CMake's `-DEVE_QSPI_ENABLE=OFF` is different: the shared recipe then omits the C definition.

**Sources:** [Configuration header][config-h], [shared CMake options][examples-cmake].

Do not select an integrated MCU/display module merely because its screen resolution matches. Module selections can also select module specific touch or LCD-driver behaviour. Similarly, the current `EVE_settings.h` contains display-timing selection tables: a genuinely new timing requires deliberate support there, not an assumption that arbitrary `EVE_DISP_*` definitions will override those tables.

**Source:** [Derived configuration][settings-h].

**BT82x note:** A new platform macro is not automatically covered by the existing BT82x transport defaults. Configure the parameters described in [Section 9](#int-and-interrupt-based-co-processor-completion) before attempting a BT82x build.

**_Checkpoint:_** All translation units see the same device, display, transport method and feature selection.

## 6. Integrate the source files and build configuration

### Vendor IDE or manually maintained build

For an MCU-path build, add the following common library implementation files, along with the new MCU port:

```text
source/EVE_API.c
source/EVE_HAL.c
source/extensions/bt82x_patch.c         # Only applicable to BT82x
source/extensions/custom_touch_fw.c     # Only applicable if required and to FT81x/BT88x/BT81x
source/extensions/lcd_panel_init.c      # Only applicable LCD panels that require driver initialisation
ports/eve_arch_newmcu/EVE_MCU_NEWMCU.c
```

The extension files contain conditional implementations; retain the appropriate current files rather than copying older generated application-note sources. The shared repository recipe includes these extensions as part of `eve_library`. 


**Source:** [common library source list][examples-cmake].

For the current simple example, also add:

```text
examples/simple/common/eve_example.c
examples/simple/common/eve_fonts.c
examples/simple/common/eve_images.c
examples/snippets/touch.c
examples/simple/newmcu/main/main.c
```

These are the current example sources. Add the target's board-support files, SDK sources/libraries, startup code and linker configuration separately.

**Source:** [simple-example source list][simple-cmake].

Supply these include locations as applicable:

```text
examples/simple/newmcu/
include/
examples/simple/common/
examples/snippets/
examples/simple/newmcu/board/
<vendor SDK and generated board include directories>
```

Define `PLATFORM_NEWMCU` consistently for the relevant targets and build configurations. Do not compile a donor MCU implementation alongside the new one. For a manually assembled MCU project, use `EVE_HAL.c`, not the Linux SPI-device HAL, and leave `USE_LINUX_SPI_DEV` undefined.

An existing port may split its implementation across several files. For example, the STM32 CUBE port uses [`EVE_MCU_STM32CUBE_SPI.c`][stm32-spi-c] for its conventional SPI transport and [`EVE_MCU_STM32CUBE.c`][stm32-common-c] for timing, PD#, INT# and byte-order helpers. Copying only the SPI file does not provide the complete interface. Include every required file for the selected transport variant, but do not link both the SPI and Quad SPI implementations as competing definitions.

### Reuse the current CMake structure

The current library example structure separates three concerns:

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

The directory layout matters because the shared recipe derives paths from the example platform directory. Follow the SDK's required initialisation order; a toolchain must not be introduced after the first `project()` has already selected the compiler.
    
**Sources:** [Shared recipe][examples-cmake], [Pico integration][pico-cmake], [CMake source addition][cmake-target-sources].

A single CMake file can be utilsed to compile an example, as seen in the FT900 simple example [CMakeLists.txt][ft900-cmake].

After supplying the MCU-specific SDK integration, an illustrative command-line build is:

```sh
cmake -S examples/simple/newmcu -B build/simple-newmcu \
  -DCMAKE_TOOLCHAIN_FILE=/absolute/path/to/toolchain.cmake \
  -DEVE_DEVICE=EVE_BT817 \
  -DEVE_DISPLAY_RES=EVE_WVGA \
  -DEVE_COPRO_METHOD=EVE_COPRO_CMD_WRITE

cmake --build build/simple-newmcu
```

Replace the toolchain and display selections with the actual ones. For a module-based build, use the appropriate `EVE_MODULE` instead of competing device/panel/resolution options.

**Current CMake detail:** When selecting device and resolution separately, omit the `EVE_MODULE` cache argument. Even the literal value `EVE_NO_MODULE` is a non-empty string that takes the shared recipe's module branch and prevents its device/panel/resolution arguments from being forwarded. Clear an old cached module selection or use a fresh build directory when changing selection style. Also, arbitrary `cmake -DNAME=value` arguments are not automatically C preprocessor definitions; unsupported options need explicit compile definitions or a suitable local configuration.

**Source:** [Option forwarding][examples-cmake].

**_Checkpoint:_** The new target links with exactly one MCU implementation and the intended effective configuration.

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
                 -> SPI-width / EVE INT# configuration, when enabled
            -> remaining EVE display/touch/extension configuration
       -> eve_calibrate()
       -> eve_init_fonts()
       -> eve_load_images()
       -> eve_display()
```

This sequence follows the [simple example][simple-c], [EVE initialisation implementation][api-c] and [HAL initialisation][hal-c]. Do not add an unconditional `MCU_Init()` in `main()` simply because an older example may have placed it there: the current normal startup path already calls it.

**LCD-driver ordering matters:** With `EVE_LCD_INIT` defined, the current HAL calls `lcd_driver_init()` **before** `MCU_Init()`. The applicable filename is `lcd_panel_init.c`; its callable function is `lcd_driver_init()`. Any clocks, GPIO access or time base required by that panel driver must therefore be prepared by early board setup or in the `lcd_panel_init.c` file. 

**Sources:** [Extension declaration][lcd-h], [HAL startup][hal-c].

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

`board_initialise()` is an illustrative application helper, not a required library API. Preserve the MCU vendor's entry-point signature or task model when it differs from this bare-metal example. Ensure there is only one `main()` function in the application.

### Supply the calibration-storage hooks

The simple example uses the calibration functions in [`examples/snippets/touch.c`][touch-c]. Its storage callbacks are declared in [`touch.h`][touch-h]. They are application/platform hooks, not additional functions to implement in `MCU.h`, and they are unrelated to the Linux `Platform.h` transport interface.

For initial testing without persistent storage, place the following in `main.c` or a separate application support file that includes `touch.h`:

```c
int8_t platform_calib_init(void)
{
    /* Persistent calibration storage is not implemented. */
    return 1;
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

Provide all three callbacks. In the current generic calibration path, the write callback can still be reached after interactive calibration even when the storage initialisation callback reports unavailable storage. These stubs permit recalibration without falsely reporting that uninitialised calibration data is valid. Some selected panel/module configurations supply predefined transforms and bypass interactive calibration.

**Sources:** [Callback contract][touch-h], [calibration control flow][touch-c].

### Make failures observable

Use a debugger, GPIO milestones or a working serial channel before relying on library logs. `EVE_DEBUG_LEVEL` controls verbosity only where an output mapping exists; the current [`EVE_debug.h`][debug-h] does not automatically know how to print for an arbitrary new platform macro. Add an appropriate mapping or use independent board diagnostics. Do not select an unrelated platform macro to borrow its logger.

**_Checkpoint:_** Execution reaches the EVE startup path once, and failures can be located without guessing from a blank screen.

## 8. Bring up the display in stages

### Stage A: Confirm EVE boot

Use breakpoints or diagnostic markers around `MCU_Init()`, the HAL's device-identification polling and `MCU_Setup()`. If the port returns from `MCU_Init()` but never reaches `MCU_Setup()`, inspect reset, power, SPI framing and device selection before changing display-list code.

The current HAL expects an identification value of `0x7C` for EVE devices read from `REG_ID`. It uses the selected generation's register definitions and different boot/read sequences for EVE API 1-4 and EVE API 5. Do not hard-code the EVE register address or its read framing into a supposedly device-independent MCU port.

**Source:** [Generation-specific boot implementation][hal-c].

### Stage B: Display a message without touch or asset loading

Temporarily call a minimal diagnostic function instead of `eve_example()`. This removes calibration, image and custom-font data from the initial test. Call it once after early board setup; do not call both initialisation paths in succession.

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

This uses the current public API and the same list-submission pattern as the simple example. `EVE_LIB_EndCoProList()` ends submission; explicitly wait for completion afterwards. Do not assume `EVE_LIB_BeginCoProList()` waits for a previous list to finish. Utilise the `EVE_LIB_AwaitCoProEmptyTimeout()` function call instead of the similar `EVE_LIB_AwaitCoProEmpty()` function call to test timeout functionality and EVE API transfer conformity. A zero timeout selects an unbounded wait, so use a non-zero diagnostic timeout deliberately.

**Sources:** [Public API][eve-h], [list implementations][api-c], [simple example][simple-c].

A successful message displayed on the panel establishes more than a successful compile: it exercises boot, register access, co-processor commands and visible display output. It does not yet validate every transfer size, touch, persistent storage or high-speed operation.

### Stage C: Check memory transfers

Select a confirmed unused, suitably aligned region of `RAM_G` and perform a small write/read-back test outside an active co-processor list. Do not blindly use address zero or a fixed donor-example offset: display assets and fonts may already occupy memory. The API functions `EVE_LIB_ReadDataFromRAMG()`, and `EVE_LIB_WriteDataFromRAMG()` can be used for this, and do not require wrapping in a co processor command block such as the following:
```c
EVE_LIB_BeginCoProList();
/* Commands to issue to EVE */
EVE_LIB_EndCoProList(); 
EVE_LIB_AwaitCoProEmpty();
```

Use distinctive patterns to reveal missing bytes and byte-order errors, then increase the tested length across the driver's internal transfer boundaries. Use accesses supported by the selected EVE generation; do not assume the older byte/halfword register tests apply unchanged to BT82x. The [HAL memory operations][hal-c] is the reference for the active implementation.

Treat this as a separate diagnostic, not an alteration to the application's asset layout. Restore any temporarily used memory before resuming an example that owns it.

### Stage D: Run the complete simple example

Restore the normal call to `eve_example()`. Complete touch calibration when requested, then verify the logo, font/image rendering and touch-sensitive counter. The current example waits for a touch tag before updating the counter; an unchanged number without a touch is not, by itself, evidence of a failed SPI port.

**Source:** [Current example loop][simple-c].

If the minimal message works but the full example fails, concentrate on the added paths: touch calibration, asset loading, larger transfers, available memory and co-processor completion. Do not immediately rewrite the already working boot sequence.

**_Checkpoint:_** Cold boot, visible output, memory transfers and the complete simple example work at the conservative SPI speed.

## 9. Add optional features

Introduce one feature at a time and repeat the staged tests after each change.

### Increase SPI speed, DMA and shared-bus operation

Once the basic SPI implementation is working reliably, the port can be further optimised by increasing the SPI clock, introducing DMA or queued transfers, or sharing the SPI peripheral with other devices.

#### Increase SPI speed

The initial port should use a conservative SPI clock during bring-up. Once communication, display initialisation and memory transfers have been verified, `MCU_Setup()` can be used to increase the SPI frequency to the intended operating rate.

When increasing the SPI speed, verify:

* the maximum SPI frequency supported by the selected EVE device;
* the limits of the MCU SPI peripheral;
* the electrical characteristics of the PCB, cabling and display connection;
* reliable operation of both read and write transfers;
* long or repeated transfers without corruption; and
* repeated cold starts and EVE initialisation at the selected operating speed.

Do not select the SPI frequency solely from the maximum value accepted by the MCU SDK or peripheral. The final operating rate must be suitable for the complete MCU-to-EVE interface.

#### DMA and queued transfers

DMA or queued SPI transfers may be introduced to improve throughput, but they must preserve the behaviour expected by the `MCU_*` interface.

In particular, transfer ordering must remain unchanged and any source or destination buffer must remain valid until the transfer has completed. Some scalar MCU functions pass the address of a local variable to the block-transfer routines, so an asynchronous implementation must not retain that pointer after the MCU function returns.

Where asynchronous transfers are used, either:

* complete the transfer before returning from the `MCU_*` function; or
* copy the data into storage owned by the MCU port and manage its lifetime until the transfer completes.

Also account for any MCU-specific DMA requirements such as cache maintenance, buffer alignment, memory-region restrictions and transfer-size limits.

#### Shared SPI buses and RTOS operation

When EVE shares an SPI peripheral with another device, or when the application uses an RTOS, access must be protected at the **whole EVE transaction** level.

The protected region must include:

* CS# assertion;
* address or command transfers;
* all associated read or write data transfers; and
* CS# deassertion.

Protecting only the individual SPI read or write calls is insufficient, as another task or device could otherwise gain access to the bus while an EVE transaction is still in progress.

For initial port development, it is recommended that only one task or execution context owns the EVE API. Re-entrant access, EVE operations from interrupt service routines, or concurrent access from multiple tasks require an application-specific concurrency design in addition to the basic MCU port.

These optimisations should be introduced only after the basic blocking SPI implementation has been fully verified, so that performance or concurrency changes can be tested independently from the initial porting work.

### Quad SPI

When adding Quad SPI support to a new port, the MCU implementation must provide both the required hardware wiring and the host-side interface switching needed by the library. This is additional functionality beyond the basic single-SPI port and should normally be added only after standard SPI communication has been verified.

Enable `EVE_QSPI_ENABLE` only when the selected EVE device supports Quad SPI and the MCU port has been updated to handle the required multi-line transfers. The MCU implementation must provide `MCU_SetSPIMode(uint8_t mode)` behind the same feature guard.

`MCU_SetSPIMode()` is responsible for changing the MCU-side SPI interface to match the mode requested by the HAL. Depending on the MCU, this may require reconfiguring the SPI peripheral, changing the direction or function of the data pins, and switching between single- and quad-line operation.

The HAL is responsible for configuring the EVE device itself before requesting the corresponding MCU-side interface change. `MCU_SetSPIMode()` should therefore configure only the MCU peripheral and associated pins; it should not write EVE registers or call back into the HAL to change the EVE interface mode.

When adding Quad SPI support, verify that:

* the selected MCU peripheral supports the required single- and quad-line operating modes;
* IO0 to IO3 are connected correctly and can be configured for the required directions;
* `MCU_SetSPIMode()` correctly handles each interface mode requested by the library;
* read and write transfers use the correct number and direction of data lines;
* CS# remains under the transaction control required by the library; and
* switching interface width does not introduce unintended CS# transitions or corrupt an active transaction.

If the MCU peripheral does not support one of the requested modes, `MCU_SetSPIMode()` should return a failure rather than silently continuing with an incorrect configuration.

Do not assume that an MCU peripheral or SDK intended for serial flash devices will automatically match the transfer and transaction requirements used by EVE. The resulting SPI framing and pin-direction behaviour should be verified against the EVE interface requirements.

Quad SPI support should normally be introduced only after single-SPI operation, display initialisation and basic memory transfers are working reliably. This helps isolate any failures caused specifically by the wider interface or mode switching.

**Sources:** [Host-width interface][mcu-h], [HAL mode sequencing][hal-c], [FT9XX Quad SPI implementation][ft9xx-c].

### Persistent calibration storage

After touch is working, replace the temporary callbacks with an implementation appropriate to the MCU's non-volatile memory. Store and retrieve the declared `struct touchscreen_calibration`; the common touch code manages the calibration marker and transform values. Report failure when no valid stored record can be read. The [RP2040][rp2040-c] port can be referenced for how this may be implemented. 

**Sources:** [Stored record and callback declarations][touch-h], [calibration ownership][touch-c].

Reserve storage through the target's memory/linker arrangement. Do not reuse another MCU's hard-coded flash offset. Check erase/program granularity, alignment, endurance, power-loss behaviour and any restrictions on executing from flash while it is being written. Keep this support in the application/platform storage layer rather than adding flash assumptions to the common EVE library.

### BT82x transport parameters

The current `MCU.h` supplies BT82x related defaults through a list of recognised platform macros. A new `PLATFORM_NEWMCU` is not automatically included. Supply suitable `EVE_SPI_MAX_TRANSFER` and `EVE_SPI_TIMEOUT` values for the new platform macro. Defining them only inside the new MCU `.c` file is insufficient.

**Source:** [BT82x defaults][mcu-h].

For an initial implementation that remains at **1 MHz single SPI**, the following values are a starting example, not a setting to retain without checking other speeds:

```c
#define EVE_SPI_MAX_TRANSFER 4
#define EVE_SPI_TIMEOUT      8
```

`EVE_SPI_TIMEOUT` is a **byte count in the BT82x read protocol**, not milliseconds. Recalculate/validate the read window against the actual SPI clock and interface width, including the speed selected by `MCU_Setup()`. `EVE_SPI_MAX_TRANSFER` controls the relevant HAL read chunking; it is not a promise that every call into the MCU driver will be that small. The current boot path, for example, requests a 128-byte block read.

**Sources:** [Parameter definitions][mcu-h], [BT82x read and boot operations][hal-c].

Keep these parameters distinct from `EVE_HAL_CHUNK_SIZE`, which is a common HAL chunking setting. Review stack use and SDK transfer limits, and use the current names.

**Sources:** [HAL configuration][hal-h], [MCU compatibility definitions][mcu-h].

Also verify the actual BT82x memory/display configuration, including `EVE_RAM_G_CONFIG_SIZE`, and retain the applicable patch extension in the build. Do not port the [BRT_AN_062][an062] note's older EVE boot protocol into the new MCU implementation. 

**Sources:** [Device configuration][config-h], [BT82x extension][bt82x-c].

### Custom touch support

When adding custom touch support to a new port, no additional MCU SPI primitives are normally required. The custom touch firmware is loaded through the existing EVE HAL and MCU transport functions, so a working implementation of the standard `MCU_*` interface should already provide the communication required.

The main considerations are therefore to ensure that:

* the `custom_touch_fw.c` extension is included in the build;
* any required custom touch firmware image is present for the selected hardware; and
* sufficient EVE communication has already been verified before testing the touch firmware loading process.

`EVE_CUSTOM_TOUCH` may be enabled explicitly in `EVE_config.h`, or enabled automatically by the derived module or panel configuration in `EVE_settings.h`. The selected configuration should remain consistent with the actual display module and touch controller fitted to the hardware.

Where a new module or panel requires custom touch firmware that is not already supported, additional configuration and firmware data may need to be added to the library and generated using the [EVE Asset Builder][eab] utility. This should remain within the custom touch extension and configuration layers rather than introducing touch-specific behaviour into the MCU SPI implementation.

Custom touch support should normally be added only after the basic EVE interface and display output have been verified. This makes it easier to distinguish a transport or SPI issue from a touch-controller or firmware-loading issue.

Do not disable custom touch support merely to obtain a successful build where the selected module or panel requires it, as the display may initialise correctly while the touch interface remains unavailable.

**Sources:** [Configuration][config-h], [derived module settings][settings-h].

### LCD panel initialisation

Some EVE based modules require an external LCD panel controller to be configured in addition to the normal EVE initialisation sequence. When adding support for such a module to a new port, the MCU must provide any GPIO, SPI and timing functionality required by the panel initialisation routine.

When `EVE_LCD_INIT` is enabled, the `lcd_panel_init.c` extension provides this functionality through `lcd_driver_init()`. The supplied implementation targets a specific supported panel controller and should not be treated as a generic driver for arbitrary LCD controllers.

When adding LCD panel initialisation support to a port, verify that:

* the selected module or panel configuration correctly enables `EVE_LCD_INIT`;
* `lcd_panel_init.c` is included in the build;
* the MCU provides the GPIO, SPI and delay functionality required by the panel driver;
* any panel-specific reset, chip-select and control pins are configured correctly;
* the required command sequence matches the LCD controller fitted to the module; and
* the EVE SPI configuration is restored before normal EVE communication begins if the same peripheral is shared.

The LCD panel controller may share the same SPI peripheral as EVE provided that it has a separate chip-select signal and both devices can be controlled independently. Alternatively, the panel may use a separate SPI peripheral or a GPIO bit-banged interface.

When a shared SPI peripheral is used, ensure that both devices are deselected while changing SPI configuration and that the settings required by EVE are restored before the EVE interface is used again. The panel chip select must remain separate from the EVE CS# signal.

The initialisation order is also important. With `EVE_LCD_INIT` enabled, `lcd_driver_init()` is called before `MCU_Init()`. Any MCU clocks, GPIO access, timing services or other resources required by the LCD panel driver must therefore already be available at this point, either through the earlier board initialisation or within the panel-driver implementation itself.

Where a new module uses a different LCD controller, additional panel-specific command data and possibly MCU-specific support may need to be added to the extension. These changes should remain within the LCD panel initialisation path rather than introducing panel-specific behaviour into the generic EVE SPI transport.

LCD panel initialisation should be tested independently before enabling further optional features, so that panel-controller configuration issues can be distinguished from EVE communication or display-timing problems.

**Sources:** [LCD extension][lcd-c], [LCD extension declaration][lcd-h].

### INT# and interrupt-based co-processor completion

To add the EVE INT# co-processor completion method, wire the signal, configure it as an input and select `EVE_COPRO_INT` for the `EVE_COPRO_METHOD` define in `EVE_config.h`. The derived configuration enables the library's interrupt-related path. An MCU interrupt service routine is not required simply to use this completion mechanism: the current HAL samples `MCU_Int()` while waiting.

**Sources:** [method selection][settings-h], [HAL completion wait][hal-c].

`MCU_Int()` reports the **physical level of the active-low INT# input**, not a Boolean "interrupt pending" value. For a supported input, use the following return values:

| INT# pin level | Interrupt state | `MCU_Int()` return value |
| --- | --- | --- |
| Low | Asserted | `0` |
| High | Deasserted | Non-zero, normally `1` |

This contract is documented in [`MCU.h`][mcu-h] and [`HAL.h`][hal-h]. `HAL_WaitCmdFifoEmpty()` waits while `MCU_Int()` is non-zero, then checks the interrupt flags for `EVE_INT_CMDEMPTY`.

**Source:** [HAL completion logic][hal-c].

For example, using an illustrative GPIO read function that returns `0` for a low input and `1` for a high input:

```c
int MCU_Int(void)
{
    /* Preserve the physical INT# level: low means asserted. */
    return gpio_read(int_pin);
}
```

`gpio_read()` is an illustrative placeholder for the GPIO input function provided by the target MCU SDK. Replace it with the appropriate platform-specific GPIO read operation. Do not invert the raw pin level. If the SDK instead returns a logical "interrupt asserted" state, convert that result to the physical pin-level convention expected by `MCU_Int()`.

For a polling-only implementation without a usable interrupt pin, an explicit unsupported result can be combined with a build-time rejection of the interrupt method:

```c
#if !defined(EVE_USE_CMDB_METHOD) && defined(EVE_USE_INTERRUPT_METHOD)
#error "EVE_USE_INTERRUPT_METHOD (EVE Interrupt pin) is not supported on this port."
#endif

int MCU_Int(void)
{
    return -1; /* Unsupported; this must not enter the interrupt wait loop. */
}
```

The public interrupt-input API documents `-1` as unsupported; this is not a valid sampled pin level or an automatic fallback to polling. Retain the build-time rejection when using this stub: the HAL's non-zero test would otherwise keep waiting until a configured timeout, or indefinitely when no timeout is supplied.

**Sources:** [Public interrupt API][eve-h], [HAL completion logic][hal-c].

For a wired implementation, use the GPIO implementation instead of the stub and remove the rejection once tested. Before enabling interrupt-based completion, observe both the idle and asserted INT# states and verify the corresponding return values. Then repeat a co-processor completion test using a non-zero diagnostic timeout call; `EVE_LIB_AwaitCoProEmptyTimeout(1000u)`.

`MCU_Int()` should sample the host input without changing CS#, performing EVE SPI transactions, clearing EVE interrupt flags or waiting for an asserted state. Close the submitted co-processor list before waiting. Avoid arbitrary diagnostic reads of the clear-on-read EVE interrupt flags when interrupt management is in use; use the library's intended handling so pending events are not consumed unexpectedly. 

**Sources:** [HAL completion logic][hal-c], [public interrupt-management contract][eve-h].


### Advanced chip-select implementations

The standard `EVE-MCU-Dev` MCU interface assumes that CS# is controlled independently of the SPI peripheral. For a conventional MCU port, CS# should therefore be assigned to a GPIO output and controlled explicitly through `MCU_CSlow()` and `MCU_CShigh()`.

This is important because a single EVE transaction may contain several separate calls to `MCU_SPIWrite()`, `MCU_SPIRead()` or the scalar SPI helper functions. CS# must remain asserted across those calls until the library ends the transaction.

A peripheral or SDK which automatically deasserts CS# after every byte, word or buffer transfer cannot therefore be mapped directly onto the standard MCU SPI functions, as this would divide a single EVE transaction into multiple SPI transactions.

Hardware-managed chip select may still be used in an advanced port if the implementation preserves the transaction boundaries required by the library. The MCU layer must treat the sequence between `MCU_CSlow()` and `MCU_CShigh()` as a complete logical transaction, rather than treating each transfer function as an unrelated hardware transaction. During the corresponding on-wire transaction, CS# must stay asserted across the required address and data phases.

For write-only transactions, this may require the MCU implementation to buffer or defer individual SPI operations and perform the complete hardware transaction when the library indicates that the transaction has ended. A buffered implementation may delay physical CS# assertion until transmission actually begins; it must still preserve the library's transaction boundaries and ordering.

**Reads cannot simply be deferred until `MCU_CShigh()`.** A read function must supply valid data before returning because its caller may use that data immediately. A buffered implementation must therefore execute the queued address/write phase and the requested read in the correct order, completing the read before returning. It must also preserve any remaining transaction requirements rather than assuming that every read is the final operation. This follows from the scalar read interface and the HAL's use of returned values.

**Sources:** [Read interface][mcu-h], [HAL memory reads][hal-c].

Any such implementation must preserve:

- the transaction boundaries defined by `MCU_CSlow()` and `MCU_CShigh()`;
- the order of all SPI write and read operations, including write-to-read transitions;
- the exact byte stream and number of bytes required by each transaction;
- source-buffer lifetime, by copying deferred writes into port-owned storage when necessary;
- valid read results before the read function returns, with no later access to expired destination buffers; and
- completion of the final SPI transfer before physical CS# deassertion, with all remaining queued writes complete before `MCU_CShigh()` returns.

Plan for buffer limits as well as short transactions. Do not flush a full buffer as an unrelated hardware transaction if that would introduce an invalid CS# transition or lose the current address context. Document and validate any additional protocol-aware adaptation instead of assuming it is equivalent to GPIO-controlled SPI.

The [STM32 Quad SPI implementation][stm32-qspi-c] is a reference for a more specialised design: it buffers write data, flushes pending writes from `MCU_CShigh()`, and performs reads through a separate receive path. Study it as an architectural example, not as evidence that automatic per-call chip select is suitable for a conventional SPI port or that it can be copied without target-specific validation.

This is more complex than the normal GPIO-controlled implementation and should only be used where the MCU peripheral or driver architecture requires it. For a new port, independently controlled GPIO chip select is the recommended starting point.

## 10. Troubleshooting

Use the failed checkpoint or last known working stage to narrow the investigation. Avoid changing several parts of the port at once, as this can make the original fault more difficult to identify. The checks below are diagnostic suggestions; several different faults can produce the same visible symptom.

| Symptom                                                                        | Checks                                                                                                                                                                                                                                                                    |
| ------------------------------------------------------------------------------ | ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| Missing `MCU_*` symbols                                                        | Confirm that all source files required by the selected port are compiled, including any common timing, GPIO or byte-order implementation files. Check that the platform guard matches the build definition and that function signatures match the current `MCU.h`.        |
| Duplicate symbols                                                              | Look for two MCU implementations being linked, a second `main()` function, or common sources being added both directly and through an existing library target.                                                                                                            |
| Wrong EVE configuration appears to be used                                     | Confirm that all translation units see the intended `EVE_config.h`, `EVE_defs.h` and derived configuration. Check include-directory ordering and remove stale build or CMake cache data where necessary.                                                                  |
| Missing calibration callbacks                                                  | Add all three `platform_calib_*` functions using the declarations in `touch.h`, or provide the required platform-specific persistent calibration implementation.                                                                                                          |
| `EVE_Init()` does not progress to `MCU_Setup()`                                | Check EVE power, PD#, SPI mode, clock frequency, physical pin mapping, CS# framing and the selected EVE device. Confirm that `MCU_Init()` succeeds and that EVE can respond to the initial register accesses.                                                             |
| EVE identification fails or an unexpected `REG_ID` value is returned           | Check read framing, MISO direction, byte order, SPI mode, CS# timing and the selected device configuration. Verify the signals with a logic analyser before modifying the higher-level EVE code.                                                                          |
| Reads are consistently all zeros or all ones                                   | Inspect MISO direction and continuity, EVE power and reset state, generated read clocks and CS# assertion. Check the applicable EVE read protocol rather than adding arbitrary dummy bytes in the MCU layer.                                                              |
| CS# pulses between address and data, or after every byte/buffer                | Configure CS# as an independent GPIO and disable automatic peripheral or SDK chip-select control. For a conventional port, only `MCU_CSlow()` and `MCU_CShigh()` should determine the EVE transaction boundary.                                                           |
| Data is shifted or corrupted                                                   | Check for unintended CS# transitions, extra or missing dummy bytes, incorrect scalar byte order, RX overrun, transfer-length truncation or incorrect SPI frame width. Run the mock-transport byte-order tests described earlier in this guide.                            |
| Final bytes are missing or unreliable                                          | Confirm that transmission has completed on the physical SPI interface before `MCU_CShigh()` raises CS#. Do not assume that an empty software queue, FIFO write or DMA-complete notification necessarily means that the final bit has left the peripheral.                 |
| Communication works at the initial SPI speed but fails after `MCU_Setup()`     | Reduce the SPI frequency and verify the actual clock with test equipment. Check EVE device limits, MCU peripheral timing, wiring, signal integrity and any speed-dependent BT82x read parameters.                                                                         |
| Small transfers work but image or font loading fails                           | Test transfer sizes around the MCU/SDK limits. Check buffer splitting, pointer lifetime, stack usage, DMA restrictions, memory allocation and whether CS# remains asserted when a transfer is divided internally.                                                         |
| First initialisation works but subsequent reset or re-initialisation fails     | Check the final states left by `MCU_Deinit()`, PD# and CS#, peripheral shutdown/reinitialisation behaviour, and whether the SDK permits the SPI/GPIO resources to be initialised more than once.                                                                          |
| Registers respond but the screen remains blank                                 | Verify the selected module, panel and display timing configuration, backlight control, panel power and any required external LCD-controller initialisation. Confirm that the display configuration matches the actual hardware.                                           |
| Display works but touch does not respond                                       | Check the selected module/panel touch configuration, touch-controller wiring, required custom touch firmware and calibration path. Confirm that custom touch support has not been disabled for hardware which requires it.                                                |
| Touch calibration appears to hang                                              | Verify that valid touch coordinates are being reported, that the touch controller is correctly configured and that the display orientation matches the calibration setup. Test the display without touch-dependent logic first.                                           |
| Counter in the simple example does not change                                  | The example waits for the expected touch tag before updating the counter. Confirm that touch and tag reporting work before treating the static counter as an SPI transport failure.                                                                                       |
| Single SPI works but Quad SPI fails                                            | Check IO0-IO3 wiring, MCU pin directions, `MCU_SetSPIMode()` behaviour and the point at which the interface width changes. Confirm that both the MCU and EVE are using the same interface mode and that CS# framing remains correct.                                      |
| Polling works but INT# completion hangs                                        | Check INT# wiring, pin direction and `EVE_COPRO_METHOD`. Verify that `MCU_Int()` returns `0` for asserted/low and non-zero for deasserted/high, and ensure that an unsupported-input stub is not being used.                                                              |
| LCD panel initialisation fails while normal EVE SPI works                      | Check that the GPIO, timing and SPI resources required by `lcd_driver_init()` are available before `MCU_Init()`. If the panel and EVE share an SPI peripheral, verify that each has an independent chip select and that the EVE SPI configuration is restored afterwards. |
| BT82x build lacks transport parameters, or reads fail after changing SPI speed | Define suitable `EVE_SPI_MAX_TRANSFER` and `EVE_SPI_TIMEOUT` values for the new platform and validate them against the actual SPI clock and interface width.                                                                                                              |
| DMA or RTOS operation introduces intermittent corruption                       | Check transfer completion, cache maintenance, buffer alignment and lifetime, and ensure that the complete EVE transaction is protected from concurrent SPI access rather than protecting only individual read/write calls.                                                |
| No EVE debug output                                                            | Verify the MCU debug or serial output independently. Add an appropriate mapping for the new platform in `EVE_debug.h`; changing `EVE_DEBUG_LEVEL` alone does not provide a platform-specific output function.                                                             |

Implementation references for these checks include the [MCU interface][mcu-h], [HAL implementation][hal-c], [simple example][simple-c], [touch implementation][touch-c] and [debug interface][debug-h].

When debugging a new port, return to the lowest-level failing test first. If raw SPI and GPIO behaviour is incorrect, changing display lists, touch configuration or application code is unlikely to resolve the underlying problem.

## 11. Validate and document the completed port

Before treating a new port as complete, verify the following on the actual target hardware.

### Build and configuration

* [ ] Debug and release builds complete using the intended platform configuration and exactly one MCU implementation.
* [ ] All translation units use the intended `EVE_config.h`, `EVE_defs.h` and derived EVE configuration.
* [ ] No donor-platform definitions, source files, pin assignments or assumptions remain in the new port.
* [ ] The selected `EVE_DEVICE`, `EVE_MODULE`, `EVE_PANEL`, `EVE_DISPLAY_RES` and optional feature settings match the hardware being tested.
* [ ] Any required external SDK, startup, linker or board-support dependencies are included and documented.

### MCU and hardware interface

* [ ] Cold power-up and MCU reset produce predictable CS#, PD# and SPI pin states.
* [ ] `MCU_Init()`, `MCU_Setup()` and `MCU_Deinit()` perform their intended operations and report failure correctly where applicable.
* [ ] PD# produces the expected EVE reset/power-down behaviour.
* [ ] When INT# is used, it is configured as an input and its asserted and deasserted levels have been verified on the hardware.
* [ ] Repeated initialisation or shutdown/reinitialisation works correctly where the application requires it.

### SPI transport

* [ ] For a conventional SPI port, CS# is configured as an independently controlled GPIO and is not automatically toggled by the SPI peripheral or SDK.
* [ ] CS# remains asserted across separate address, data and write-to-read operations that form one EVE transaction.
* [ ] CS# is not deasserted until the final SPI bit has completed.
* [ ] SDK buffer splitting or internal transfer limits do not introduce additional CS# transitions.
* [ ] `MCU_SPIWrite()` and `MCU_SPIRead()` transfer the requested number of bytes without adding protocol framing.
* [ ] Zero-length transfers are harmless.
* [ ] Scalar transfer and byte-order tests, including the three-byte address representation, produce the expected byte streams.
* [ ] Read and write transfers work correctly over a range of sizes, including lengths around any MCU or SDK transfer limits.
* [ ] The basic display, RAM_G read-back tests and complete simple example operate at both the initial and selected final SPI frequencies.

### Timing and stability

* [ ] `MCU_Delay_20ms()` and `MCU_Delay_500ms()` provide at least the required delays.
* [ ] `MCU_Time_ms()` advances correctly and remains valid after any MCU clock-frequency changes.
* [ ] Repeated cold starts and extended operation do not introduce intermittent SPI or display failures.
* [ ] Operation remains reliable at the selected final SPI frequency rather than only at the initial conservative frequency.
* [ ] Error or timeout behaviour can be observed and does not falsely report successful communication when EVE is disconnected or unresponsive.

### Optional features

Only validate features which are supported by the new port, but each enabled feature should be tested on hardware rather than merely compiled.

* [ ] Persistent touch calibration can be written, read back and rejected correctly when invalid.
* [ ] Custom touch firmware loads successfully where required by the selected module or panel.
* [ ] Quad SPI correctly switches between the interface modes requested by the HAL and transfers data reliably.
* [ ] INT# co-processor completion works using the documented `MCU_Int()` return convention.
* [ ] LCD panel initialisation completes correctly and leaves the EVE SPI interface in the expected state.
* [ ] BT82x-specific transport parameters have been validated at the selected SPI frequency and interface width.
* [ ] Shared-bus or RTOS access cannot interleave separate EVE transactions.
* [ ] Any DMA or queued implementation preserves transfer ordering, buffer lifetime and transaction completion.
* [ ] For an advanced hardware-managed chip-select implementation, reads return valid data before the MCU read function returns, queued writes complete at the required transaction boundary, and buffer-boundary conditions do not alter the intended EVE transaction.

### Port documentation

The port README should document:

* the supported MCU, development board and SDK/toolchain versions;
* the EVE devices or modules used during validation;
* SCK, MOSI, MISO, CS#, PD# and INT# pin assignments;
* the independent CS# GPIO assignment, or details of a validated advanced chip-select implementation;
* initial and final SPI frequencies;
* build, programming and debugging instructions;
* required SDK or external dependencies;
* any board setup which must occur before `EVE_Init()`;
* supported optional features such as Quad SPI, INT#, persistent calibration or LCD-panel initialisation;
* known limitations or unsupported features; and
* the EVE-MCU-Dev revision or commit used during validation.

A useful completion criterion is:

**Another developer should be able to build the example, connect the documented hardware and reproduce the basic display and touch tests without modifying the common EVE API or HAL to compensate for MCU-specific transport behaviour.**

Any change required in the common EVE API, HAL or configuration framework should be identified separately from the MCU port itself and validated across the existing supported platforms where applicable.


## Source reference index

The primary references are [BRT_AN_062][an062] for the staged porting workflow and the [EVE-MCU-Dev source][source] for the active implementation. Start with [MCU.h][mcu-h], [EVE_HAL.c][hal-c], [EVE_API.c][api-c], [EVE_config.h][config-h] and [EVE_settings.h][settings-h] when checking the interface against a newer checkout.

For MCU implementation examples, use the [RP2040 port][rp2040-c] or the [STM32 SPI implementation][stm32-spi-c] together with its [common helpers][stm32-common-c]. The [STM32 Quad SPI port][stm32-qspi-c] illustrates an advanced buffered transport rather than the basic GPIO-controlled pattern.

For build integration, use [examples/examples.cmake][examples-cmake], [simple/common.cmake][simple-cmake] and the [Pico platform example][pico-cmake]. For application startup and storage, use [simple/common/eve_example.c][simple-c], [touch.h][touch-h] and [touch.c][touch-c]. Source-specific links are also provided beside the relevant instructions throughout this guide.

[source]: ./
[an062]: https://brtchip.com/wp-content/uploads/2024/04/BRT_AN_062_Porting_BRT_AN_025_to_NXP_MCU.pdf

[readme]: README.md
[ports-readme]: ports/README.md
[simple-readme]: examples/simple/README.md

[mcu-h]: include/MCU.h
[hal-h]: include/HAL.h
[hal-c]: source/EVE_HAL.c
[eve-h]: include/EVE.h
[api-c]: source/EVE_API.c

[config-h]: include/EVE_config.h
[defs-h]: include/EVE_defs.h
[settings-h]: include/EVE_settings.h
[debug-h]: include/EVE_debug.h

[rp2040-c]: ports/eve_arch_rpi/EVE_MCU_RP2040.c
[stm32-spi-c]: ports/eve_arch_stm32/EVE_MCU_STM32CUBE_SPI.c
[stm32-qspi-c]: ports/eve_arch_stm32/EVE_MCU_STM32CUBE_QUADSPI.c
[stm32-common-c]: ports/eve_arch_stm32/EVE_MCU_STM32CUBE.c
[ft9xx-c]: ports/eve_arch_ft9xx/EVE_MCU_FT9XX.c
[nxp-c]: ports/eve_arch_nxpk64/EVE_MCU_NXP.c
[4222-c]: ports/eve_libft4222/EVE_libft4222.c
[mpsse-c]: ports/eve_libmpsse/EVE_libmpsse.c
[emulator-c]: ports/eve_emulator/EVE_emulator.c

[examples-cmake]: examples/examples.cmake
[simple-cmake]: examples/simple/common.cmake
[pico-cmake]: examples/simple/pico/CMakeLists.txt
[pico-port-cmake]: ports/eve_arch_rpi/pico.cmake
[ft900-cmake]: examples/simple/ft900/CMakeLists.txt

[simple-c]: examples/simple/common/eve_example.c
[touch-h]: examples/snippets/touch.h
[touch-c]: examples/snippets/touch.c

[lcd-h]: include/extensions/lcd_panel_init.h
[lcd-c]: source/extensions/lcd_panel_init.c
[bt82x-c]: source/extensions/bt82x_patch.c

[cmake-target-sources]: https://cmake.org/cmake/help/latest/command/target_sources.html

[eab]: https://brtchip.com/eab/
