---
name: eve-mcu-dev-project-builder
description: Create or extend EVE-MCU-Dev applications using existing supported ports and examples. Use for new examples, combining example features, changing the EVE device or display configuration, adding resources, or diagnosing application integration problems. Use the separate porting skill when support for a new MCU, platform, SDK, or toolchain must be implemented.
---

# EVE-MCU-Dev Project Builder

Build maintainable EVE applications from the repository's existing examples and supported ports.

## Establish the target

Before making hardware-dependent changes, resolve the choices that affect the request:

- Application behaviour
- Exact EVE device or module
- Existing supported host platform and build system
- Display or panel configuration
- Touch configuration
- Resource storage requirements
- Output location and acceptance criteria

Ask one focused question when a missing choice would materially change the implementation. Otherwise use a repository-supported default and report it.

## Select the baseline

1. Read the repository `AGENTS.md`.
2. Inspect the working tree and preserve unrelated changes.
3. Use `examples/simple` as the application baseline.
4. Read [sample-selection.md](references/sample-selection.md) when borrowing features from another example.
5. Read [compatibility-checks.md](references/compatibility-checks.md) when the task depends on a particular EVE device, display, touch controller, interface, storage method, or toolchain.
6. Read [known-errors.md](references/known-errors.md) when diagnosing or reviewing an integration failure.

For a new application, create a new example directory. Do not modify existing examples, snippets, library sources, or existing ports.

For a non-trivial combination, record which functions, data and dependencies will be borrowed. Keep one owner for EVE initialization, the main loop, touch handling, coprocessor access, bitmap handles and RAM_G addresses.

## Implement

- Reuse existing library APIs, build patterns and platform abstractions.
- Copy only the required application code and resources.
- Keep portable application code separate from platform-specific setup.
- Make resource addresses, sizes, alignment and bitmap handles reviewable.
- Preserve ANSI C compatibility unless the target requires otherwise.
- Do not silently change pins, SDKs, toolchains, timing, binary blobs or dependencies.
- Stop and report evidence when the requested combination is unsupported.

For bitmap, font or storage choices, follow established patterns in `simple` first, then inspect the relevant examples identified in `sample-selection.md`.

## Validate

Apply the layers relevant to the task:

1. Review effective macros, initialization, resources and build inputs.
2. Configure and build the exact requested target.
3. Use the emulator when it represents the requested behaviour.
4. Clearly separate emulator, build and physical-hardware evidence.
5. Preserve baseline behaviour and shared repository files.

Never claim hardware validation unless the named hardware was actually tested.

## Report

Summarize the selected baseline, target configuration, changed files, validation performed, limitations and remaining hardware checks. When blocked, report the first actionable error or exact missing decision.
