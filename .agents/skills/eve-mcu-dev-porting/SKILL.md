---
name: eve-mcu-dev-porting
description: Add a new MCU or host-platform port to EVE-MCU-Dev, including transport code, build integration, a simple example target, documentation, and validation. Use for new platform support, not for applications using an existing port.
---

# EVE-MCU-Dev Porting

Add a minimal, maintainable MCU or Platform port without changing the EVE API
or HAL behavior.

## Establish the target

1. Locate the repository root and read the applicable `AGENTS.md`.
2. Inspect the branch and working tree. Preserve unrelated changes.
3. Identify:
   - exact MCU, board, SDK, and toolchain;
   - MCU port (`MCU.h`) or operating-system Platform port (`Platform.h`);
   - SPI peripheral and SCK, MOSI, MISO, CS#, PD#, and optional INT# pins;
   - intended EVE device/module and display configuration for validation;
   - available persistent storage, if any.
4. Verify hardware facts using the target vendor’s current documentation.
5. Ask only for missing information that would materially change the port.

Consult the relevant sections of `PORTING.md` for repository layout, interface
contracts, build integration, bring-up, and troubleshooting.

## Select a baseline

Choose the closest current port by SDK and transport behavior. Do not assume a
particular reference port is always appropriate.

Inspect:

- the selected port implementation and build recipe;
- the declarations in `MCU.h` or `Platform.h`;
- the matching target under `examples/simple`;
- the selected SDK’s SPI, GPIO, timing, and storage APIs.

Reuse patterns, not whole directories. Do not modify existing ports or the
`simple/common` sources.

## Implement the port

Create a unique `PLATFORM_*` macro and guard every port source file with it.

The implementation must:

- implement every required function declared by `MCU.h` or `Platform.h`;
- use blocking transfers unless a deliberately designed buffered transport is
  required;
- preserve CS# across the complete transaction defined by the HAL;
- keep CS# under independent GPIO control for a conventional SPI port;
- transfer exactly the requested bytes without adding EVE framing;
- supply the required delays and a continuously advancing millisecond counter;
- preserve the target’s byte order correctly;
- return failures from lifecycle functions when initialization fails.

Reject unsupported optional features, such as Quad SPI or INT# completion, at
build time rather than silently misconfiguring them.

Keep pin assignments, frequencies, SDK choices, and storage locations explicit.
Do not invent hardware details.

## Add the simple example target

Add a new platform directory under `examples/simple` containing the required
build files and application entry point.

Provide:

- `platform_calib_init`;
- `platform_calib_read`;
- `platform_calib_write`;
- `platform_get_time`.

Use persistent storage only when it can be reserved safely. Otherwise return a
documented failure so interactive calibration can still be used.

Do not add another `main()` when the SDK already supplies one.

## Validate

Perform every applicable check:

1. Review platform guards, definitions, pins, byte order, initialization order,
   and exactly one selected port implementation.
2. Configure the exact board, toolchain, EVE device, and display target.
3. Build the complete `simple` application.
4. Confirm the expected firmware outputs are produced.
5. Provide a hardware smoke test for SPI, EVE boot, display, touch, INT#, and
   storage as applicable.
6. Clearly distinguish compilation from actual hardware validation.

Never claim hardware validation unless it was performed on the named hardware.

## Report

Summarize:

- selected baseline and adaptations;
- host, SDK, EVE, display, interface, and storage configuration;
- files created or changed;
- configure and build results;
- unsupported features and remaining assumptions;
- required hardware tests.