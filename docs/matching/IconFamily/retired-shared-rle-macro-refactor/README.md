# Retired shared RLE macro refactor

This directory preserves the complete experimental shared-decoder source shape
retired on 2026-07-27. `IconMacro.h` is the shared tiered macro implementation;
the ten `.cpp` files are the exact call sites that used it.

The experiment recovered useful semantic blocks and exact CFG-topology islands,
but it regressed every affected decoder below its previously retained fuzzy
maximum. Most importantly, it changed the audited exact `IconToBitmapYModify`
implementation from 100% to 79.479700% in the clean object.

The live sources were restored from commit `a41bb40d`, the checkpoint
immediately before `IconMacro.h` was introduced. Future work should use the
exact `IconToBitmapYModify` implementation as the structural reference and
transfer its developer-plausible blocks into siblings as ordinary per-TU C++.
The archived macro experiment remains available as a source of semantic block
boundaries, naming, and decoder-dialect comparisons; it is not matching source.
The explicit recovery campaign is specified in
`../semantic-structure-recovery-plan.md`.
