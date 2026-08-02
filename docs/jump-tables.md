# Switch jump-tables: mechanism and the landed comparison-side normalization

**Status: resolved for scoring.** The historical artifact described below capped
switch/dispatch functions slightly under 100% no matter how correct the reconstruction
was. It is handled since the embedded-switch-table pass in
`scripts/homm2/build/canonicalize_data_symbols.py` (see
[`data-symbol-normalization.md`](data-symbol-normalization.md), "Embedded switch
tables"): both objects are compared through disposable normalized copies under
`build/objdiff/normalized/`, where label-relative table references are rewritten to a
common spelling. Membership in `build/gen/jump_tables.csv` no longer implies a score
cap; a jump-table function that stays non-exact has an ordinary reconstruction
residual.

## Mechanism (raw objects — still true, still worth knowing)

MSVC lowers a dense `switch` to `jmp dword ptr [4*reg + disp32]` through a table of
case-label addresses. In MSVC's own object the table is a compiler-internal static label
(`$Lnnnnn`, storage class 6, inside the owning function's `.text` range); the dispatch
reads it at **disp 0** with a DIR32 relocation on the disp field naming the `$L` symbol,
and each 4-byte entry carries a DIR32 to a case label:

```
88d: ff 24 8d 00 00 00 00     jmpl *(,%ecx,4)
     00000890: IMAGE_REL_I386_DIR32  $L39184        <- table label, disp 0
```

No `$L` label survives anywhere (the image is stripped), so the delinked retail object
cannot know the table's identity. `vostok-delinker` folds the table into the function and spells every
reference as `function + K` (dispatch disp = K = table offset, entry addends = case
offsets):

```
890: ff 24 8d 67 02 00 00     jmpl *0x267(,%ecx,4)
     00000893: IMAGE_REL_I386_DIR32  ?HandleCastSpell@@YIHAAUtag_message@@@Z
```

objdiff masks relocation symbols but not the stored disp bytes, so comparing the raw
objects would make every dispatch a guaranteed mismatch (`00 00 00 00` vs `K`).

## The landed fix (candidate-side normalization, not the delinker)

The normalization pass rewrites, in the disposable comparison copies only, every
same-function `.text` DIR32 relocation through a local label (type 0, scl 6) or through
a function-plus-interior-addend into the **unambiguous containing function**, adjusting
the encoded addend so the resolved section offset is unchanged. After the pass, both
sides spell the dispatch identically (`disp = K`, reloc → function) and the disp bytes
agree. Raw compiler and delinker objects remain authoritative for linking, disassembly,
and the hard gates; only the comparison copies are rewritten.

Verified example: `?HandleCastSpell@@YIHAAUtag_message@@@Z` (SOURCE/SPELLS) — normalized
base and target dispatches are byte-identical, `objdiff-cli diff` with the repository's
`functionRelocDiffs=data_value` strictness reports 100.0 with zero diff rows, and the
function scores exact.

## Why the delinker-side route was abandoned (history)

Two synthetic-PDB attempts tried to teach the delinker the table identity:

1. `S_LDATA32` at the table address — the delinker drops data/label symbols landing in
   `.text`; they never surface.
2. `S_GPROC32` at the table address — produced the desired disp-0 reloc but sliced the
   table into its own chunk and shrank the owning function's extent, which the
   inventory-size-based comparison punishes (scores got worse), and mid-function tables
   cannot be sliced at all.

The comparison-side rewrite achieves the same equivalence with no delinker change and
no extent risk.

## Detector (kept; useful beyond scoring)

`scripts/homm2/build/detect_jump_tables.py` scans `build/delink/**/*.c.obj` for
`ff 24 8d <disp32>` dispatches and runs of 4-byte-spaced DIR32 self-relocs, writing
`build/gen/jump_tables.csv` (`func_rva,func_name,table_rva,table_size`). Regenerate with
`python3 -m homm2.build.detect_jump_tables`. When reconstructing a large `switch`,
recover body order separately from case values and compare the ordered table
destinations against the detector output.

## Open observations (not currently blocking)

- **Tail tables** (table after the final `ret`, e.g. `?Main@listBoxWidget@@…`): the
  claimed size includes the table while our compiled function symbol ends at the code,
  so extents differ between the raw objects. Re-examine if such a function reaches
  instruction-exactness yet refuses the exact gate.
- Delinked objects can carry a duplicate undefined symbol record for a function that is
  also defined in the same object (observed for `?HandleCastSpell@…`). Harmless to
  scoring today.
