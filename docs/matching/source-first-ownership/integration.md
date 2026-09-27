# Source-first ownership checkpoint

Historical checkpoint: the [whole-module follow-through](follow-through.md)
records the subsequent REQUEST cleanup, restored DIMMER header dependency,
and fresh measurements.

This supersedes the 115-byte checkpoint's source model. Misc uses direct
literals/catalog references, DIMMER is an ordinary concrete class in one TU,
and the Audiere node has a private header definition with straightforward
constructor assignments and an implicit destructor. The destructor-only TU,
its special archive partition, and the template-only ownership models are gone.
No object or executable layout corrections are applied.

The [two whole-game passes](two-passes.cpp) cover 24 complete source arrangements.
The [compiler and archive controls](header-first-use.cpp) distinguish inline body
visibility from emitted-copy ownership and final placement. None recovers the
retail layout. The simple source is retained as a research checkpoint; the old
115-byte source arrangement is available in history, not claimed as current.

## Fresh validation, 2026-09-27

Validated in `.claude/worktrees/matcher-4`, branch `matcher/native-link-recovery`,
after merging `decomp-gold-2.1-buka` at
`30941650e64dd0da9abf1c57291d3a963565337b` and resolving the conflicts. The main
checkout and its concurrent matching work were not changed.

`homm2 redelink`, `homm2 build`, the historical native link, status refresh,
tool selftests, clean export/standalone Win32 build, and `git diff --check`
completed. The tool suite ran 1,004 tests with seven skipped. Integration also
fixes the annotation scanner's failure to recognize a `VA(...)` following a
same-line template declaration, with a regression test covering lexical noise.

The normalized object comparison reports **1,724/1,727 functions at 100%** and
**291,986/291,990 data bytes**. The three absent functions are DIMMER's generated
ctype initializer helpers (5, 39 and 18 bytes); the four absent data bytes are
their CRT initializer cell. Narrowing DIMMER's include dependency removed this
incidental emission. AudiereEffects reports all 17 compared functions and all
32 data bytes at 100%; DIMMER's seven methods/wrapper and vtable also compare
at 100%. These comparison results do not establish final-image placement or
complete ordered relocation equality.

The 1,208,393-byte native image has SHA-256
`8f3fef432648fd9fe6f8e9a654580c967bc71636c691fce9dc760e97ce60c9e4`.
There are **290,183 differing bytes at fixed file offsets**: headers 264,
`.text` 115,890, `.rdata` 484, `.data` 173,545. Resources match. This count
includes widespread address and storage movement after removing the former
source arrangement; it is not 290,183 independent instruction errors.

The strict native audit correctly exits 1. It finds 1,468/1,509 ordinary source
starts at retail RVAs, with 41 displaced; 1,659/1,727 project starts agree, with
66 displaced and two unavailable. All 47 Misc ordinary starts still agree.
Import ABI and IAT order agree; semantic import bytes are 6,374/6,374 and thunk
bytes 244/244, although raw thunk layout differs.

Focused relocation-owner multiset checks report Purge 12/12, Play 40/40 and
DIMMER destructor 2/2, without missing owners. The broader BASE addend audit
compares 561 functions, finds zero value-set or count-only mismatches, but
exits 1 with 855 one-sided identity rows across 144 functions (including 526
canonical-data rows in 78 functions). Raw compiler labels versus canonical
retail labels account for many unresolved rows. This is **not** a completed
ordered relocation proof, and the checkpoint makes no such claim.

Logs and raw evidence are under `build/holistic-source-first/`:
`final-{redelink,build,link,status,audit}.log`, `selftest.log`, `clean.log`,
`{purge,play,dimmer}-relocs.log`, `base-addends.log`, and
`retained-address-deltas.json`. The clean export built successfully with the
standalone Win32 toolchain; no runtime/play-test result is claimed.

This checkpoint updates the draft PR into the decomp branch. It does not claim
propagation to master or Ironfist, or completion of executable matching.
