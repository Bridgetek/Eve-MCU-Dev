# Sample selection

Use the current repository as the authoritative inventory; names and support can change.

## Selection strategy

1. Start with `simple` example.
2. Choose the example that already builds for the requested MCU or Platform and EVE generation when possible.
3. Prefer a smaller example over a visually similar but dependency-heavy demo.
4. Inspect the example's source and build files before recommending it. A folder name alone is not compatibility evidence.

## Typical discovery areas

Search for these categories rather than assuming exact paths:

| Need | First search area | Also inspect |
| --- | --- | --- |
| Drawing primitives | simple | Display-list construction and coordinate scaling |
| Default widgets | cruise | Touch/tag handling and coprocessor FIFO usage |
| Bitmap or image | simple, racecar, spaced_invaders | Format, stride, layout, handles, RAM_G use |
| Fonts and Unicode | simple, fontmagic, gyminterval | Font format, glyph storage, target-generation support |
| Touch | simple, touchscreentest | Touch controller, calibration, tags, custom firmware |
| Sound or audio | gyminterval | Sample format, playback ownership, storage bandwidth |
| Video | flightdeck-bt820 | Media FIFO, storage source, codec and IC support |
| External flash | racecar, medinfo | Blob/header, flash state, alignment and address map |
| Power management | N/A | Wake source, clocking and platform GPIO behavior |
| Dashboard or instrument UI | flightdeck, submarine, racecar | Layout, state model, animation and resource cost |

## Reuse map

Create a reuse map before editing:

| Feature | Baseline/pattern source | Code reused | Dependencies | Adaptation | Evidence |
| --- | --- | --- | --- | --- | --- |

Keep one primary baseline. For every secondary example, identify the specific function, data, or pattern to reuse. Do not copy its complete initialization or main loop without reconciling ownership.

## Combination rules

- Use one EVE initialization path.
- Use one application loop or explicit scheduler.
- Assign one owner to touch polling and event dispatch.
- Assign one owner to the coprocessor FIFO.
- Plan a single RAM_G and external-flash address map.
- Reconcile bitmap handles and font handles.
- Load shared resources once unless lifecycle requirements say otherwise.
- Separate portable application code from host-specific transport and GPIO code.
- Preserve the repository's existing HAL boundary.