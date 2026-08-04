# Solving a whole-frame slot permutation: map the slots, then move the suffix

**Trigger.** Instruction streams align 1:1 (same count, same opcodes, same
branch shape) yet dozens of rows differ only in `-0xN(%ebp)`. The residual is a
permutation of the named-local layout, i.e. a set of `/Od` name-hash buckets
this branch's invented identifiers do not reproduce.

Do **not** grind spellings. The permutation is fully determined and the solve is
mechanical.

## Step 1 - derive the correspondence from the objects

`build/scratch-m1/m1_slotmap.py` aligns the two disassemblies on a key that
*erases* ebp displacements (`-0x[0-9a-f]+(%ebp` -> `-0xD(%ebp`) and then votes,
at every aligned instruction pair, on `ours_displacement -> retail_displacement`.

```
$ HOMM2_DIR=$PWD python3 build/scratch-m1/m1_slotmap.py \
      SOURCE/VIEW '?HandleViewGeneral@@YIHAAUtag_message@@@Z' 0x286
aligned 167/172 ours rows (171 retail rows)
  -0x4     -> -0x14     (1/1)
  -0x8     -> -0x10     (1/1)
  ...
  -0x28    -> -0xc      (3/3)
```

A unanimous vote (`n/n`) is a proof for that slot; a split vote means the
alignment is still broken by a *structural* difference and must be fixed first.
Cross it with `homm2 sema frames <UNIT> <fn>` (our own `/Z7` frame) to turn the
displacement map into a **name -> retail slot** table.

## Step 2 - choose buckets, not names

`/Od` orders locals by `(bucket ascending, declaration index descending)` where
`bucket = od_slots.bucket(name)`. Read retail's slot order off step 1 and assign
each role a bucket so the sequence is non-decreasing. With `N <= 16` locals the
easiest solve is *strictly increasing* buckets `0..N-1`: declaration order then
stops mattering entirely.

## Step 3 - keep the stem, move the suffix

This tree's reconstructed identifiers already carry disambiguating suffixes
(`msgConst59`, `windowLines_7`, `heroCombatResult3`). Search suffixes of the
*same stem* for the required bucket; the source keeps reading the same and the
change is honest about what is evidence (the order) and what is not (the name).

```python
for c in [stem] + [f'{stem}{n}' for n in range(0, 400)]:
    if c not in used and bucket(c) == target_bucket:
        return c
```

`SOURCE/VIEW`'s `HandleViewGeneral` needed all fifteen widget-id locals moved:

```
  bucket  0  msgConst19  -> msgConst4      bucket  8  msgConst31 -> msgConst17
  bucket  1  msgConst25  -> msgConst3      bucket  9  msgConst59 -> msgConst18
  bucket  2  handled3    -> handled28      bucket 10  msgConst26 -> msgConst15
  bucket  3  msgConst0   -> msgConst1      bucket 11  msgConst66 -> msgConst9
  bucket  4  msgConst    -> msgConst8      bucket 12  msgConst65 -> msgConst0
  bucket  5  msgConst32  -> msgConst7      bucket 13  msgConst27 -> msgConst28
  bucket  6  msgConst71  -> msgConst6      bucket 14  msgConst28 -> msgConst19
  bucket  7  msgConst72  -> msgConst5
```

99.19% -> EXACT in one edit. The same three steps closed `ViewGeneral`
(twenty locals, four bucket ties broken by declaration order), `DoArenaDialog`
(sixteen, including four dead scalars replacing a dead 12-byte array),
`DoSurrender` (four renames), `PlaySmacker` (a two-line declaration swap) and
the six-local inner block of `game::ViewArmy`.

## Caveats

- **Fix structure first.** A single missing statement misaligns the tail and
  poisons the votes; work top-down and re-run step 1 after every structural fix.
- **Dead locals are real slots.** `DoArenaDialog`'s retail frame has four
  4-byte holes where our source declared one `i32[3]` plus one `i32`; the array
  had to become four scalars before the buckets could line up.
- **Scope groups still exist.** A local declared in a nested block slots after
  the whole enclosing group whatever its bucket - `game::ViewArmy` needed
  `iconWidget* spellWidget0;` hoisted out of the `for` body before its bucket
  had any effect.
