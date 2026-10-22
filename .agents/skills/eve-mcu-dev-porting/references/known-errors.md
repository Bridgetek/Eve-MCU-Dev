# Known integration errors

Use this as a review checklist, not as proof of root cause. Confirm every diagnosis from code, logs, registers, and hardware evidence.

| Symptom or risk | Check first | Safe response |
| --- | --- | --- |
| Blank or unstable display | Full LCD timing, clock source, PD_N sequence, display enable, rotation | Compare every timing field with a validated configuration; change one variable at a time |
| Coprocessor command abort | Exact EVE command support, FIFO alignment/space, preceding command stream, arguments | Capture the first failing command and error report; reset only after preserving evidence |
| Project builds but feature does nothing | Wrong generation guard, target macro, omitted resource, uncalled state path | Trace the selected build definitions and runtime entry path |
| CRC or identity check failure | SPI mode/clock, CS/PD_N, power, byte order, expected data range | Reduce SPI speed, verify signals and repeat deterministic reads before blaming memory |
| Flash stays BASIC or detached | Correct blob/header, flash device support, wiring, command sequence | Verify flash ID/status and use the blob intended for the exact EVE/flash combination |
| Corrupt bitmap/font | Format, stride, layout, source address, alignment, handle reuse | Recalculate addresses and sizes; inspect converted metadata rather than guessing |
| Touch coordinates wrong | Controller type, rotation, calibration matrix, display size | Validate raw coordinates first, then transformation and UI hit regions |
| Touch stops after combining samples | Duplicate polling, tag ownership, custom firmware initialization | Centralize touch initialization and event routing |
| Control looks active but does not respond | Missing/zero tag, tag mask disabled too early, visual larger than tagged geometry, wrong event type | Draw the complete hit area under one nonzero tag and trace press, debounce, release, long-press, and page-lock handling together |
| Text flickers or disappears only on hardware | External-flash scanout contention, several large glyph rows, hidden content still rendered | Measure the target's underrun diagnostic by page/state; reduce same-row traffic, cache or copy hot assets to RAM_G, and omit fully obscured content |
| Space is missing or displays another glyph | Incomplete subset, zero advance, visible/aliased space glyph, mismatched font metadata and glyph asset | Regenerate with explicit space support and validate U+0020 before and after flash packing |
| Video stalls or aborts | IC/codec support, media FIFO, storage throughput, command order | Validate a minimal video example on the same hardware before integration |
| SD/file resource fails on target | Missing physical storage, driver, filesystem, path semantics | Select supported storage or add an explicit platform storage implementation |
| Random corruption after adding a feature | RAM_G overlap, buffer overflow, handle collision, resource lifetime | Produce a complete memory and handle map and check boundaries |
| Linker duplicate symbols | Wholesale copied initialization/HAL files | Reuse shared code and keep one implementation owner |
| Wrong platform code compiled | Stale CMake cache or conflicting target defines | Inspect the effective compile definitions and configure a clean build directory when safe |
| Works in emulator only | Emulator-specific filesystem, timing, touch, or unsupported hardware path | Separate simulation evidence from hardware evidence and run a hardware smoke test |

## Review gates

Before declaring completion, confirm:

- The first error in the log was investigated rather than a later cascade.
- The selected build definitions name the intended host, EVE IC, and display.
- EVE initialization occurs once.
- Coprocessor FIFO access has one owner or explicit serialization.
- Resource address ranges and bitmap/font handles do not overlap.
- Every file-backed feature has a valid storage implementation on the target.
- Simulator and hardware results are reported separately.
