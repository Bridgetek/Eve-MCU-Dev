---
name: eve-mcu-dev-porting
description: Add new MCU or Platform support to EVE-MCU-Dev. Use when a user wants to add support for a new MCU or Platform to EVE-MCU-Dev and add the port to the repository. Verify every hardware and generation-specific claim from the repository and Bridgetek documentation rather than assuming compatibility.
---

# EVE-MCU-Dev Porting

Add support for a new MCU or Platform to EVE-MCU-Dev while preserving the repository's boundaries.

## Start with evidence

1. Locate the EVE-MCU-Dev repository root and read its nearest `AGENTS.md` files.
2. Inspect the current branch and working tree. Preserve unrelated user changes.
4. Treat the checked-out source and its build files as the authority for names, paths, targets, and supported combinations. Use current Bridgetek documentation when the repository does not establish a hardware fact.
5. Never infer that a feature supported by one EVE generation is supported by another.

Read the bundled references as needed:

- Before creating a new port, read `PORTING.md` and follow its directory, naming, build-registration, documentation, and validation requirements.
- Read `./references/compatibility-checks.md` whenever the task changes or depends on an EVE device, host, SPI mode, display, touch controller, storage medium, or toolchain.
- Read `./references/known-errors.md` before implementation review and when diagnosing a failure.

## Collect the project profile

- Before porting EVE-MCU-Dev to an MCU or Platform that has no existing host implementation decide how to implent the port:
  - For MCU ports use `ports/eve_arch_rpi/EVE_MCU_RP2040.c` as the embedded reference, but verify the new MCU's SPI pin multiplexing, SDK, timer, and GPIO behavior from its hardware documentation.
  - For Platform ports on Linux platforms use `ports/eve_arch_beaglebone/EVE_Linux_BBB.c` as the embedded reference, but verify the new Linux platform's SPI deviceaccess rules, SPI pin multiplexing, SDK, timer, and GPIO behavior from its hardware documentation.
  - For other ports verify the new device's SPI methods, SDK, timer, and GPIO behaviour from its hardware documentation. If the SPI bus required buffered reading or writing then use `ports/eve_libft4222/EVE_libft4222.c` as the embedded reference; otherwise use `ports/eve_arch_rpi/EVE_MCU_RP2040.c` as the embedded reference.

## Select the baseline

1. Inspect the `simple` example as the primary method for testing. Treat other ports as pattern sources, not folders to merge wholesale.
2. Record a short reuse map containing:
   - Required feature
   - Source example and relevant files or functions
   - Dependencies
   - Required adaptation
   - Compatibility evidence

Do not modify the `common` directory in the `simple` example.
Add a new directory to the `simple` example to support the new port. 
Populate this directory with all build files required to compile the `simple` application for the new port. Follow the directory format of other similar ports.

The C files generated must include functions for calibration storage and millisecond timer detailed in `PORTING.md`.

- `int8_t platform_calib_init(void)`
- `int8_t platform_calib_write(struct touchscreen_calibration *calib)`
- `int8_t platform_calib_read(struct touchscreen_calibration *calib)`
- `uint32_t platform_get_time(void)`

If there is a clear method to use persistant storage on the MCU or Platform then populate the `platform_calib_*` functions with minimal code that can read and write persistent storage, and return zero from `platform_calib_init()`. If not then return -1 from all `platform_calib_*` functions.

If there is a clear method to obtain a millisecond clock on the MCU or Platform then populate the `platform_get_time()` function with minimal code to obtain a millisecond counter.

The application must return zero for normal program completion and non-zero if an error was detected.
Unless a competing `main.c` or file containing a `main()` function is supplied by the SDK used in the port, create a `./main/main.c` for the application `main()` function and above functions. 

## Plan before editing

The objective of a port is to connect the common EVE-MCU-Dev library to the target platforms SPI, GPIO and timing facilities, without modifying the API or HAL layers. 

The required features are:

* Blocking SPI reads and writes. These can be in units of 8-bits or a block of up to 16 kB. Multiple smaller blocks can be used to make a larger block.
* Predictable control over the chip select (CS#) signal to the SPI. 
  * Do not use the SPI peripheral automatic chip select function unless the whole block can be buffered before sending.
* Two GPIO output lines:
  * One for controlling chip select (CS#) on the SPI bus.
  * Another for setting the power down (PD#) pin to reset the EVE device.
* A timer function for a 20 ms delay and one for a 500 ms delay.
* A millisecond counter for use in detecting timeouts.
* A macro prefixed with `PLATFORM_` to uniquely identify the port.

Optional features are:
* One input GPIO line:
  * An input signal for interrupt signal (INT#) from the EVE device.
* Support for QuadSPI reads and writes.

### Port Required Files

The port must have C files which can be compiled to implement the prototypes in the include files [`MCU.h`][mcu-h] for MCU ports or [`Platform.h`][platform-h] for Platform ports.

All port C files must be guarded by `#if` or `#ifdef` preprocessor conditions with the Platform Macro to prevent the port code being compiled for other platforms.

A port must provide functions as described in the following table. Note the different file names for MCU and Platform ports.

| Group | MCU Ports | Platform Ports | Responsibility |
| --- | --- | --- | --- |
| Lifecycle | `int MCU_Init(void)`, `int MCU_Setup(void)`, `int MCU_Deinit(void)` | `int Platform_Init(void)`, `int Platform_Setup(void)`, `int Platform_Deinit(void)` | Configure, adjust and release the host interface. Return `0` on success and `-1` on failure. |
| Control pins | `MCU_CSlow()`, `MCU_CShigh()`, `MCU_PDlow()`, `MCU_PDhigh()` | `Platform_CSlow()`, `Platform_CShigh()`, `Platform_PDlow()`, `Platform_PDhigh()` | Control CS# independently of the SPI peripheral and PD#. Apply the named physical levels. |
| Interrupt input | `int MCU_Int(void)` | `int Platform_Int(void)` | Return `0` for EVE INT# pin assertion (logic low) and non-zero for deassertion (logic high). |
| Block transfers | `MCU_SPIWrite(const uint8_t *, uint32_t)`, `MCU_SPIRead(uint8_t *, uint32_t)` | `Platform_SPIWrite(const uint8_t *, uint32_t)`, `Platform_SPIRead(uint8_t *, uint32_t)` | Transfer the requested bytes without changing CS# or adding protocol framing. |
| Scalar transfers | `MCU_SPIWrite8()`, `MCU_SPIWrite16()`, `MCU_SPIWrite24()`, `MCU_SPIWrite32()`, `MCU_SPIRead8()`, `MCU_SPIRead16()`, `MCU_SPIRead32()` | `Platform_SPIWrite8()`, `Platform_SPIWrite16()`, `Platform_SPIWrite24()`, `Platform_SPIWrite32()`, `Platform_SPIRead8()`, `Platform_SPIRead16()`, `Platform_SPIRead32()` | Supply the fixed-size SPI transfer operations used by the HAL. |
| Timing | `MCU_Delay_20ms()`, `MCU_Delay_500ms()`, `uint32_t MCU_Time_ms(void)` | `Platform_Delay_20ms()`, `Platform_Delay_500ms()`, `uint32_t Platform_Time_ms(void)` | Supply minimum delays functions and an advancing millisecond count. |
| Host to wire byte order | `MCU_htobe16()`, `MCU_htobe32()`, `MCU_htole16()`, `MCU_htole32()` | `Platform_htobe16()`, `Platform_htobe32()`, `Platform_htole16()`, `Platform_htole32()` | Convert host values to the specified byte order. |
| Wire to host byte order | `MCU_be16toh()`, `MCU_be32toh()`, `MCU_le16toh()`, `MCU_le32toh()` |  `Platform_be16toh()`, `Platform_be32toh()`, `Platform_le16toh()`, `Platform_le32toh()` | Convert the specified byte order to host values. |
| Optional SPI transfer width | `int MCU_SetSPIMode(uint8_t mode)` | `int Platform_SetSPIMode(uint8_t mode)` | Select QuadSPI transfer width when `EVE_QSPI_ENABLE` is defined. |

## Implement minimally

1. Reuse the repository's existing ports for similar MCUs or Platforms as templates.
2. Copy only the feature-specific code and data required by the reuse map.
3. Keep platform-specific code in the source code for the new port.
4. Preserve ANSI C compatibility unless the selected target explicitly requires another language mode.
5. Keep resource addresses, sizes, alignment, bitmap handles, and storage locations explicit and reviewable.
6. Add comments for non-obvious hardware constraints, not for obvious control flow.
7. Do not silently change SDKs, toolchains, pin assignments, display timing, flash blobs, or third-party dependencies.

When the requested feature is unsupported on the target, stop that implementation path. Report the evidence and offer a supported alternative.

## Validate in layers

Perform every applicable layer:

1. **Static review**: check includes, macros, target definitions, resource addresses, alignment, handles, command availability, and initialization order.
2. **Configure**: run the repository's actual configuration command for the selected platform, EVE IC, and display.
3. **Build**: build the exact application target. Do not claim success from configuration alone.
4. **Simulation**: run the supported emulator or simulator when it represents the selected features.
5. **Hardware**: provide or execute a concise smoke test covering boot, display, touch, resources, and the new feature. Clearly distinguish performed tests from instructions for the user.
6. **Regression**: ensure the reused baseline behavior and shared libraries were not unintentionally changed.

For a new port ask the user to test the generated code on suitable hardware to verify operation. If a feature cannot be represented in the emulator, record that limitation and verify it on the target hardware. 

Never claim hardware validation unless it was actually performed on the named hardware.

## Report the result

Summarize:

- Selected baseline and borrowed patterns
- Target EVE device, host, display, touch, interface, and storage configuration
- Files changed or created
- Configure/build/test commands and outcomes
- Compatibility evidence and remaining assumptions
- Hardware checks still required
- Known limitations or follow-up work

If blocked, report the exact missing decision, unavailable dependency, unsupported combination, or first actionable error. Do not conceal it with speculative code.
