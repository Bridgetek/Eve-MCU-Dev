---
name: eve-mcu-dev-project-builder
description: Create, extend, combine, port, or troubleshoot Bridgetek EVE-MCU-Dev projects from existing example code. Use when a user wants to create a new project; add support for EVE-MCU-Dev to an existing project; target a different MCU or Platform; target a different toolchain; change the EVE device to target; change the display resolution or panel to display on; or avoid known integration mistakes. Verify every hardware and generation-specific claim from the repository and Bridgetek documentation rather than assuming compatibility.
---

# EVE-MCU-Dev Project Builder

Construct a maintainable application from EVE-MCU-Dev examples while preserving the repository's boundaries.

## Start with evidence

1. Locate the EVE-MCU-Dev repository root and read its nearest `AGENTS.md` files.
2. Inspect the current branch and working tree. Preserve unrelated user changes.
4. Treat the checked-out source and its build files as the authority for names, paths, targets, and supported combinations. Use current Bridgetek documentation when the repository does not establish a hardware fact.
5. Never infer that a feature supported by one EVE generation is supported by another.

Read the bundled references as needed:

- Before creating a new `examples` application ask for a name for the new example.
- Read `references/sample-selection.md` before selecting or combining examples.
- Read `references/compatibility-checks.md` whenever the task changes or depends on an EVE device, host, SPI mode, display, touch controller, storage medium, or toolchain.
- Read `references/known-errors.md` before implementation review and when diagnosing a failure.

## Collect the project profile

Resolve these fields before making hardware-dependent changes:

- Desired application behavior and required features
- EVE device or module
- Host MCU or PC platform
- Toolchain, SDK, and build system
- Host interface and SPI mode, frequency, and signal mapping
- LCD or panel resolution, timing configuration, pixel clock, rotation, and color format
- Touch type, controller, and any custom firmware requirement
- Resource types and storage locations: RAM_G, EVE flash, SD card, host filesystem, "static const" C arrays, or mixed
- Debug channel, programmer, and available hardware
- Output location and acceptance criteria

Ask one focused question for any missing choice that can change architecture or compatibility. Do not block on preferences that can be safely defaulted and reported.

## Select the baseline

1. Inspect the `simple` example as the primary baseline. Treat other examples as pattern sources, not folders to merge wholesale.
2  Inspect the dependencies of the example under the `common` directory.
2. Select a matching directory in the `simple` example to select a compatible port to use.
3. Record a short reuse map containing:
   - Required feature
   - Source example and relevant files or functions
   - Dependencies
   - Required adaptation
   - Compatibility evidence

Do not modify the canonical `simple` example. Always create a new directory in the example directory for a new example. To add support for a platform to an existing example add a new directory to for the support and build files for the new port based on the port support in the `simple` example.

## Plan before editing

Describe the proposed structure briefly:

- Initialization ownership
- Screen or application state flow
- Input and touch event flow
- Resource loading and lifetime
- Media FIFO, flash, SD, or host-file ownership when applicable
- Error handling and debug output
- Build target and hardware configuration

Identify conflicts between examples before copying code. In particular, avoid duplicate EVE initialization, competing main loops, duplicate resource addresses, conflicting bitmap handles, repeated touch setup, and multiple owners of the coprocessor FIFO.

## Resource guidance

When the task uses fonts or bitmap resources:

1. Try to use the method in the `simple` example for bitmaps and fonts. Data is stored in the program code as a C array with "const static" modifiers to not store the data in the heap or stack.
2. If the MCU cannot store the bitmaps or fonts in the program code:
   - Examine the methods in the `racecar` example to select a storage methods selected by the `ASSETS` macro in the `racecar/common/EVE_example.h` file.
   - Use the file format for the bitmaps or fonts shown in the `examples/racecar/assets/` directory.
3. Confirm that the selected EVE device supports the proposed resource formats.
4. Compare visual quality, resource size, and storage usage.
5. Validate representative resources on the target LCD or panel.

When the task builds an interactive page, also verify the complete tagged hit area, the input layer's press/release/long-press semantics, conditional page states, and the longest localized content. Treat emulator rendering as functional evidence only; use target-hardware counters and observation for scanout bandwidth and touch behavior.

## Implement minimally

1. Reuse the repository's existing library and helper APIs.
2. Copy only the feature-specific code and data required by the reuse map.
3. Centralize EVE, display, touch, storage, and resource initialization.
4. Keep platform-specific code behind existing abstractions and compile-time configuration.
5. Preserve ANSI C compatibility unless the selected target explicitly requires another language mode.
6. Keep resource addresses, sizes, alignment, bitmap handles, and storage locations explicit and reviewable.
7. Add comments for non-obvious hardware constraints, not for obvious control flow.
8. Do not silently change SDKs, toolchains, pin assignments, display timing, flash blobs, or third-party dependencies.

When the requested feature is unsupported on the target, stop that implementation path. Report the evidence and offer a supported alternative.

## Validate in layers

Perform every applicable layer:

1. **Static review**: check includes, macros, target definitions, resource addresses, alignment, handles, command availability, and initialization order.
2. **Configure**: run the repository's actual configuration command for the selected platform, EVE IC, and display.
3. **Build**: build the exact application target. Do not claim success from configuration alone.
4. **Simulation**: run the supported emulator or simulator when it represents the selected features.
5. **Hardware**: provide or execute a concise smoke test covering boot, display, touch, resources, and the new feature. Clearly distinguish performed tests from instructions for the user.
6. **Regression**: ensure the reused baseline behavior and shared libraries were not unintentionally changed.

For a new application use the emulator port as an early gate when it can represent the application behavior. Build the emulator target, run a bounded startup/rendering smoke test, and run deterministic self-tests for applicable interactions and resource invariants. Resolve failures before adding RP2040-specific code and building the exact RP2040 target. Keep platform-dependent setup behind the existing HAL boundary. If a feature cannot be represented in the emulator, record that limitation and verify it on the target hardware. 

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
