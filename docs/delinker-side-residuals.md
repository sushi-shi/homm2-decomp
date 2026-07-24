# Delinker-side residuals in the exact backlog

Some live non-exact functions fail the exact gate because of how the **delinked retail
target** (`vostok-delinker` output under `build/delink/`) represents the code — the
reconstruction is byte-correct, but the target it is compared against is not what MSVC
actually emitted. These are fixed in the delinker, not in `src/`.

Scope: the 323 `no-exact` function residuals in the island-search backlog
(`/tmp/homm2-residual-campaign-results.tsv`, latest status per function). Data-object
ownership is a *separate* delinker subsystem — see
[`delinker-contribution-manifest.md`](delinker-contribution-manifest.md).

## TL;DR

- **69 of 323** residuals are **switch jump-table folds** — the one confirmed delinker-side
  class in the function backlog. Full mechanism + fix already in
  [`jump-tables.md`](jump-tables.md); this doc quantifies the backlog impact.
- Fixing the delinker jump-table handling **closes the residuals where the fold is the
  sole divergence** and **removes one guaranteed mismatch from the other ~67** (which also
  carry independent reconstruction residuals).
- **`fuzzy = 100.0000` does NOT imply a delinker issue.** Two functions that round to 100%
  (`ProcessDeath`, `SetMenus`) are *real* reconstruction near-misses, verified below. The
  reliable delinker test is the `jump_tables.csv` cross-reference, **not** the fuzzy score.

## Class 1 — switch jump-table folds (69 residuals)

**Mechanism** (byte-level detail and worked `heroWindow` / `listBoxWidget::Main` examples in
[`jump-tables.md`](jump-tables.md)): MSVC emits a `switch` jump table as its own internal
`$L` symbol, so the dispatch reads the table at **disp 0** (`jmp [4*eax + 0x0]`, reloc →
`$L`). CodeView records no `$L` label, so the delinker **folds the table into the owning
function** and rewrites the dispatch as `jmp [4*eax + K]` at **disp K** (reloc → `func`).
objdiff masks the reloc symbol but **not the displacement**, so that one `jmp` counts as a
mismatch on every switch function.

**Detector.** `scripts/homm2/build/detect_jump_tables.py` → `build/gen/jump_tables.csv`
(234 tables). A jump table is a run of 4-byte-spaced DIR32 self-relocs starting at the
dispatch's disp `K`. Reproduce: `python3 -m homm2.build.detect_jump_tables`.

**Backlog impact** (cross-referencing the 234 tables against the 323 residuals — full list in
`build/gen/delinker_jumptable_residuals.tsv`):

| fuzzy band | count | what the delinker fix does |
|---|---|---|
| ≥ 99.9% | 2 | table disp is the **sole** residual → **becomes exact** on fix |
| 99.0–99.9% | 21 | near-exact; disp mismatch dominates, tiny extra residual |
| 95.0–99.0% | 39 | table disp **plus** a real reconstruction residual |
| < 95.0% | 7 | table disp plus a substantial reconstruction residual |
| **total** | **69** | one guaranteed mismatch removed from every one |

The `≥ 99.9%` pair that should go exact on the delinker fix alone:

```
SOURCE/SPELLS   0x00020aec  100.0000  ?HandleCastSpell@@YIHAAUtag_message@@@Z     (table 0x14 = 5 entries)
SOURCE/ADVMGR   0x0005a644   99.9835  ?ProcessHover@advManager@@QAEHHH@Z          (table 0x18 = 6 entries)
```

Most affected TUs: `SOURCE/GAME` (6), `SOURCE/EVENTS` (6), `SOURCE/ADVMGR` (6),
`SOURCE/SPELLS` (5), `SOURCE/X_CAMPGN` (4). The class is dominated by message dispatchers
(`*Handler`, `*::Main`) and widget/game-mode switches — exactly where MSVC emits jump tables.

**Fix (delinker-side, from `jump-tables.md`):** for each entry in `jump_tables.csv`, emit a
**nested label at `table_rva`** and reference the dispatch (and, ideally, the table entries)
through it at **disp 0**, *without changing the owning function's CodeView extent* — mirroring
MSVC's interior `$L`. That makes the delinked `jmp` byte-identical to our compile for every
switch function in one shot. Two synthetic-PDB approaches were tried and reverted (they made
the delinker shrink the owning function → scores got worse); details in `jump-tables.md`.

## Not a delinker issue — `fuzzy ≈ 100` reconstruction near-misses

A function can round to `100.0000%` and still be a genuine reconstruction residual, because
fuzzy is instruction-count-weighted and a tiny divergence in a large function disappears in
rounding. **Do not attribute these to the delinker.** Verified counterexamples (current live
diff via `homm2 sema disasm <rva> --diff --lite`):

- **`?ProcessDeath@army@@QAEXH@Z` @ `0x51766`** (1013 bytes, /Od, no jump table) — a **basic-
  block ordering** difference plus a real **`imul` strength-reduction shape**: retail lowers a
  multiply as `lea [ecx+8*eax]` + `lea [eax+2*eax]`; our compile emits `shl; sub;
  lea [ecx+2*ecx]`. Source-side work, not the delinker.
- **`?SetMenus@@YIXPAXH@Z` @ `0x1ceb8`** (345 bytes, /Od, no jump table) — a single **branch-
  sense inversion**: our compile emits `jge` where retail has `jle` (loop condition
  reconstructed in the opposite direction). Source-side.

**Reliable delinker test:** a residual is delinker-side only if (a) its `func_rva` is in
`build/gen/jump_tables.csv`, or (b) the current masked diff shows **zero** mnemonic/operand
differences (pure address/extent/reloc-symbol), i.e. objdiff would be 100% if it also masked
the displacement/extent. Case (b) beyond jump tables was **not observed** in the ≥ 99.9%
non-jump-table residuals sampled — they were all real reconstruction diffs.

## Data-side delinker (separate subsystem, not these residuals)

`vostok-delinker`'s `.data`/`.rdata`/`.bss` **contribution ownership** — classifying writable
NB09 owner chunks, TU boundary attribution, addend derivation — is documented in
[`delinker-contribution-manifest.md`](delinker-contribution-manifest.md) and
[`static-storage-link-audit.md`](static-storage-link-audit.md). It affects *data-object*
comparison, not the function fuzzy residuals inventoried here. If a data-side audit is wanted,
that is a distinct pass.

## Reproduce

```sh
python3 -m homm2.build.detect_jump_tables          # -> build/gen/jump_tables.csv (234 tables)
# residual backlog: /tmp/homm2-residual-campaign-results.tsv (status=no-exact, latest per fn)
# cross-ref list:   build/gen/delinker_jumptable_residuals.tsv (69 rows)
homm2 sema disasm 0x51766 --diff --lite             # ProcessDeath: real residual (not delinker)
homm2 sema disasm 0x1ceb8 --diff --lite             # SetMenus: real residual (not delinker)
```
