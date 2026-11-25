# Compatibility checks

Never use a family nickname such as "pre-82x" as a concrete chip model. Resolve the exact EVE IC before selecting commands or configuration.

## Evidence order

Use the first source that establishes the required fact:

1. Current checked-out EveApps source and build configuration
2. Configured EveApps MCP server results pointing to current source or documentation
3. Official Bridgetek programming guide, datasheet, module schematic, or application note
4. User-provided validated board configuration

Record uncertainty when sources disagree. Do not convert an inference into a supported claim.

## EVE IC

- Confirm the exact model and generation.
- Check every register, coprocessor command, pixel format, font format, video/animation feature, flash operation, and touch capability used by the project.
- Verify RAM_G size and any generation-specific memory map assumptions.
- Confirm whether a patch or blob is required and that it matches the selected IC and flash device.
- Keep unsupported code behind an established compile-time guard only when the repository already supports that pattern.

## Host platform and toolchain

- Confirm that the toolchain supports every language used by the project and can link mixed C/C++ code if needed.
- Confirm the selected EveApps platform target, BSP/SDK version, startup code, linker configuration, and runtime libraries.
- Verify SPI peripheral, chip select, PD_N, optional interrupt, debug UART, timers, and storage interfaces.
- Verify filesystem assumptions. A target without SD-card or host-file access cannot run a file-backed example unchanged.

## Interface

- Distinguish single-SPI host-command operation from any supported wider transfer mode.
- Confirm signal mapping, voltage, SPI mode, byte order, maximum clock during initialization, and maximum operational clock.
- Verify mode-transition sequences from the target IC documentation and repository implementation.
- Do not describe QSPI support from a module name alone.

## LCD and touch

- Verify the complete timing set, not only width and height.
- Check pixel clock limits, sync polarities, offsets, cycle lengths, active area, rotation, and output format.
- Confirm that layout coordinates and resource dimensions match the selected rotation and resolution.
- Identify resistive, capacitive, or no-touch configuration and the exact controller.
- Confirm calibration, coordinate transformation, interrupt behavior, and custom touch firmware requirements.

## Resources and storage

- Calculate RAM_G usage with alignment and temporary buffers included.
- Assign non-overlapping addresses and handles.
- Confirm the storage source exists on the target and has adequate capacity and bandwidth.
- For EVE flash, verify device support, blob/header handling, flash state, erase/program behavior, alignment, and update strategy.
- For media FIFO, verify base, size, alignment, producer/consumer behavior, and collision with other resources.

## Compatibility decision

Classify each important requirement as:

- **Verified**: direct evidence supports the exact combination.
- **Conditional**: supported only with named configuration or adaptation.
- **Unsupported**: direct evidence rules it out.
- **Unknown**: evidence is missing or conflicting.

Do not begin irreversible or large implementation work while a critical item remains unknown.