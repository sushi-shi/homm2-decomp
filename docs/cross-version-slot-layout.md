# Why /Od stack slots move between HoMM2 2.0 and 2.1

Investigation of 2026-08-03. Everything below was measured in this session from the
retail images in `investigation/extracted/` and from probe compiles with the two
pinned toolchains. No earlier investigation was used as input.

## The question

On `decomp-gold-2.1-buka` a large number of otherwise byte-correct functions differ
from retail only in their local stack-slot displacements. A 2.0 -> 2.1 point release
should not involve mass edits, so "the devs renamed things" was not a credible cause.
`wsProcessMessages` was the worked example.

## The answer

1. **MSVC `/Od` does not assign frame slots in declaration order. It walks a 16-bucket
   per-scope symbol table, and the bucket is a hash of the identifier's spelling.**
   Rename a local and its slot moves; keep the name and the slot is fixed.
2. **The rule is identical in MSVC 4.2 (cl 10.20) and VC6 SP5 (cl 12.00.8964)**, and is
   unaffected by `/Zi`, `/Z7`, `/Gi`, declaration order, or inner-block scope. Measured,
   not assumed - see §2.
3. Therefore a slot difference between two builds of the same function is *always* a
   difference in the source token stream, never a compiler artefact.
4. **The 2.0 image is the outlier, not 2.1.** Heroes II 1.0 (Nov 1996) and Gold 2.1
   (Aug 1997) agree with each other exactly; PoL 2.0 (May 1997) is the odd one out in
   112 of 112 decidable cases, with zero counterexamples.
5. The reason is not a renaming campaign: **the three images were built from three
   different source trees**, and the divergence is concentrated in the game-logic files
   the expansion work touched. The shared engine layer (`BASE/`) is at 7% and the CRT at
   0%, while `SOURCE/` is at 34%.

So: nobody renamed 258 functions between two releases. `HEROES2W.EXE` 2.0 was built from
a *fork* (`I:\Projects\Heroes\Prog`), and Gold 2.1 was built from the mainline
(`F:\h2xsrc`) that descends from 1.0 (`D:\heroes2`). Our reconstruction inherited
identifiers fitted to the fork; on this branch they have to be re-fitted to the mainline.

---

## 1. The mechanism

`scripts/homm2/core/od_slots.py` already carried this model, reverse-engineered from the
compiler binaries. This session re-measured it end to end and it holds on VC6:

```
lexer    (C1XX):  h = 0;  for each char c of the identifier:  h = (h >> 4) + h*4 + c
                  key = (h ^ (h >> 16)) & 0xffff
backend  (C2):    bucket = key & 0xF          -- 16 buckets per scope, LIFO chains
                  the frame walk visits bucket 0..15, each chain newest-first
=> slot order (from ebp-4 downwards) = sort locals by (bucket ASC, decl index DESC)
```

The `this` spill of a `/Gr` member function is allocated after all named locals.

### The controlled experiment

`probe3.c`: five functions with **identical bodies and identical declaration order**;
only the identifiers differ. Compiled `/Od /MT /Gr /G5 /Ob1`:

| probe | locals (declaration order) | buckets | measured layout, ebp-4 first |
|---|---|---|---|
| `nameA` | attackMask, oldSide, oldIndex, j | 12, 9, 5, 10 | oldIndex -0x4, oldSide -0x8, j -0xc, attackMask -0x10 |
| `nameB` | attackMask, savedTargetSide, targetIndexSave, hexIndex | 12, 15, 15, 0 | hexIndex -0x4, attackMask -0x8, targetIndexSave -0xc, savedTargetSide -0x10 |
| `nameC` | a, b, c, d | 1, 2, 3, 4 | a -0x4, b -0x8, c -0xc, d -0x10 |
| `nameD` | zzzzzzzzzz, yyyyyyyyy, xxxxxxxx, wwwwwww | 10, 0, 11, 9 | yyyyyyyyy -0x4, wwwwwww -0x8, zzzzzzzzzz -0xc, xxxxxxxx -0x10 |

Every measured layout equals `od_slots.predict_offsets()` exactly. Note `nameB`, where
`savedTargetSide` and `targetIndexSave` collide in bucket 15 and the later declaration
takes the higher slot - the decl-index-descending tie-break.

`nameA` is Gold 2.1 / Buka's real `SetupGridForArmy` layout. `nameB` is PoL 2.0's. Same
compiler, same body, same declaration order - the identifiers alone select which retail
image you reproduce.

## 2. What was ruled out, and how

Each row is a probe compile whose output was byte-compared, not an argument.

| hypothesis | test | result |
|---|---|---|
| compiler version changed the rule | probe1-3 under cl 10.20 *and* cl 12.00.8964 | **identical layouts in all 11 probe functions** |
| debug info perturbs slots | VC6 `/Zi` and `/Z7` vs none | identical |
| `/Gi` incremental compilation | cl 10.20 `/Gi` vs `/Gi-` | identical |
| declaration order drives layout | `probeA/B/C`: struct+2 scalars in 3 different declaration orders | **all three byte-identical**; order only breaks ties inside one bucket |
| inner-block scope moves a local | `probeG` (loop var at function scope) vs `probeH` (declared inside the `while`) | identical |
| aggregate vs scalar grouping | `probeF`, `probeL` | class order is stable and the same in both compilers |
| some other bucket rule produced 2.1 | search over 3 hash variants x 17 shifts x 5 masks x tie-break x rotation x reversal, scored on 92 byte-validated functions | best alternative fits **19/92**; the standard rule fits the 2.0 side **92/92** |

The last row is the decisive one. If the 2.1 build's compiler had used a different table
size, hash slice, or walk direction, one rule would have re-sorted the *same* names into
the 2.1 order across the corpus. Nothing does.

## 3. Worked example: `wsProcessMessages`

Three locals: `sockaddr_in` (16 bytes), the address-length `int` passed to `recvfrom` by
address, and the receive-size `int`. Frame is `0x18` in every Win32 build - same three
objects, same sizes, everywhere.

| build | date | linker | function | addr-len | sockaddr | size |
|---|---|---|---|---|---|---|
| Heroes II 1.0 | 1996-11-04 | 3.10 | 0x4b4d4e | **-0x18** | **-0x14** | **-0x4** |
| demo | 1996-11-25 | 3.10 | 0x456a6e | **-0x18** | **-0x14** | **-0x4** |
| PoL 2.0 | 1997-05-05 | 3.00 | 0x407234 | -0x14 | -0x10 | -0x18 |
| PoL 2.0 (Fargus RU) | 1997-05-05 | 3.00 | 0x407234 | -0x14 | -0x10 | -0x18 |
| Gold 2.1 | 1997-08-29 | 3.10 | 0x45f910 | **-0x18** | **-0x14** | **-0x4** |
| Buka (VC6) | 2003-04-04 | 6.00 | 0x4b25c8 | **-0x18** | **-0x14** | **-0x4** |

1.0, the demo, Gold 2.1 and Buka all agree. Only the 2.0 line differs - and Buka, built
six years later with a different compiler, sits with the 1996 build.

Our source declares `remote`, `addressLength`, `receiveSize`, whose buckets are 6, 7, 14.
That sorts to `remote` -0x10, `addressLength` -0x14, `receiveSize` -0x18: exactly PoL
2.0, which is why the function matched on the PoL line and sits at 84.87% here.

To reproduce the 2.1 frame the identifiers must satisfy

```
bucket(size-var) < bucket(sockaddr-var) < bucket(addr-len-var)
```

Candidate name sets that satisfy it (398 found in a small pool of natural 1996 spellings;
`od_slots.ROLES` is the catalog to search):

| size var | sockaddr var | addr-len var | buckets |
|---|---|---|---|
| `nBytes` | `remote` | `addressLength` | 2 < 6 < 7 |
| `nBytes` | `from` | `fromlen` | 2 < 10 < 12 |
| `nBytes` | `addr` | `namelen` | 2 < 13 < 14 |
| `cnt` | `sa` | `addrLen` | 4 < 4 < 5 (tie - decl order decides) |

Note the first row: keeping `remote` and `addressLength` and renaming only the size local
to `nBytes` is sufficient. This is a one-identifier fix, not a rewrite.

## 4. Worked example: `combatManager::SetupGridForArmy`

This one is airtight because both reconstructions are proven exact against their own
retail image, and the two sources are the same text apart from identifiers:

```
homm2-decomp (PoL 2.0, cl 10.20, 100%)   homm2-buka (Buka, VC6, 100%)
    i32        attackMask;                   i32        attackMask;
    CombatSide savedTargetSide;              CombatSide oldSide;
    i32        targetIndexSave;              i32        oldIndex;
    i32        hexIndex;                     i32        j;
```

Same declaration order, same types, same body (the only other delta is one comparison
written `armyPtr->m_hex == hexIndex` vs `j == armyPtr->m_hex`, which does not touch the
frame). Measured frames:

| local | PoL 2.0 | Gold 2.1 | Buka |
|---|---|---|---|
| attackMask | -0x8 | -0x10 | -0x10 |
| saved side | -0x10 | -0x8 | -0x8 |
| saved index | -0xc | -0x4 | -0x4 |
| loop counter | -0x4 | -0xc | -0xc |
| `this` spill | -0x14 | -0x14 | -0x14 |

Gold 2.1 and Buka are identical; 2.0 differs. Both layouts are reproduced from their own
identifier sets by the bucket rule - and by `probe3`'s `nameA`/`nameB` above.

## 5. The timeline: 2.0 is the outlier

Method: for every function in the PoL 2.0 claimed inventory, mask absolute addresses and
outbound call/jump displacements, search the other image for a unique match, then compare
the ordered `[ebp-disp]` fields. `IDENTICAL` = same bytes including displacements;
`SLOT_ONLY` = same bytes, different displacements, bijective remap.

| comparison | IDENTICAL | SLOT_ONLY | body changed |
|---|---:|---:|---:|
| PoL 2.0 -> Gold 2.1 | 855 | **258** | 594 |
| PoL 2.0 -> Heroes II 1.0 | 743 | 120 | 860 |
| PoL 2.0 -> demo | 764 | 119 | 839 |

Cross-referencing the 1.0 and 2.1 censuses over the 811 functions decidable in both:

| outcome | count |
|---|---:|
| all three builds identical | 699 |
| 2.0 differs from both 1.0 and 2.1, **1.0 and 2.1 agreeing exactly** | **112** |
| 1.0 == 2.0 but 2.1 moved | **0** |
| 2.0 == 2.1 but 1.0 differs | **0** |

There is not one function where the 2.1 build introduced a layout change relative to a
1.0 == 2.0 baseline. "2.1 changed things" is exactly backwards.

## 6. Why 258 functions - the fork

The images name their own build trees, and there are four of them:

| build | tree |
|---|---|
| Heroes II 1.0 | `D:\heroes2\Source`, `D:\heroes2\Base` |
| PoL 2.0 / Fargus | `I:\Projects\Heroes\Prog\SOURCE`, `...\Prog\BASE` |
| Gold 2.1 | `F:\h2xsrc\Source`, `F:\h2xsrc\Base` |
| Buka | `e:\Users\igorl\VSS\HMM\HMM2\Source\Game`, `...\Source\Base` |

1.0 and 2.1 are the same lineage (`Source`/`Base`, and they agree on every decidable
frame). 2.0 comes from a separate tree with a different layout (`Prog\SOURCE`,
`Prog\BASE`) - the expansion work. Buka is a 2003 Visual SourceSafe checkout of the
2.1-lineage source rebuilt with VC6.

And the divergence is not spread evenly - it tracks exactly what an expansion team would
have edited:

### Divergence by layer

| layer | identical | slot-only | body changed | slot-only rate |
|---|---:|---:|---:|---:|
| BASE (engine library) | 216 | 16 | 158 | 7% |
| SOURCE (game logic) | 458 | 239 | 308 | 34% |
| EDITOR | 7 | 3 | 0 | 30% |
| CRT / runtime | 174 | 0 | 128 | 0% |

### Per-unit (units with any slot divergence)

| unit | slot-only | identical | body changed | rate |
|---|---:|---:|---:|---:|
| `SOURCE/GAME` | 28 | 37 | 33 | 43% |
| `SOURCE/ADVMGR` | 21 | 33 | 37 | 39% |
| `SOURCE/PHILAI` | 19 | 22 | 38 | 46% |
| `SOURCE/COMMAND` | 15 | 13 | 8 | 54% |
| `SOURCE/AI` | 14 | 1 | 4 | 93% |
| `SOURCE/EVENTS` | 12 | 13 | 15 | 48% |
| `SOURCE/CMBTMGR` | 11 | 15 | 11 | 42% |
| `SOURCE/HERO` | 10 | 24 | 2 | 29% |
| `SOURCE/KB` | 10 | 39 | 18 | 20% |
| `SOURCE/CURSOR` | 9 | 7 | 2 | 56% |
| `SOURCE/Newgame` | 8 | 5 | 3 | 62% |
| `BASE/RESMGR` | 8 | 15 | 2 | 35% |
| `SOURCE/PATH` | 7 | 5 | 0 | 58% |
| `SOURCE/SPELLS` | 7 | 9 | 21 | 44% |
| `SOURCE/TOWNMGR` | 7 | 17 | 9 | 29% |
| `SOURCE/wingraph` | 7 | 23 | 2 | 23% |
| `SOURCE/SWAPMGR` | 6 | 6 | 1 | 50% |
| `SOURCE/ARMY` | 6 | 24 | 11 | 20% |
| `BASE/Bzip` | 6 | 41 | 23 | 13% |
| `SOURCE/kbwin` | 4 | 8 | 2 | 33% |
| `SOURCE/VIEW` | 3 | 0 | 0 | 100% |
| `SOURCE/REMOTE` | 3 | 4 | 6 | 43% |
| `SOURCE/comwin` | 3 | 5 | 2 | 38% |
| `SOURCE/DRAWING` | 3 | 6 | 4 | 33% |
| `EDITOR/mapcell` | 3 | 7 | 0 | 30% |
| `SOURCE/ARMYGRP` | 3 | 9 | 0 | 25% |
| `SOURCE/netwin` | 3 | 13 | 3 | 19% |
| `SOURCE/RECRUIT` | 2 | 3 | 3 | 40% |
| `SOURCE/x_cdtest` | 2 | 4 | 0 | 33% |
| `SOURCE/SPELLAI` | 2 | 4 | 5 | 33% |
| `SOURCE/TOWN` | 2 | 7 | 0 | 22% |
| `SOURCE/Modem` | 2 | 12 | 1 | 14% |
| `SOURCE/Overview` | 1 | 0 | 5 | 100% |
| `SOURCE/FLY` | 1 | 0 | 2 | 100% |
| `SOURCE/x_arena` | 1 | 0 | 2 | 100% |
| `SOURCE/Campaign` | 1 | 1 | 5 | 50% |
| `SOURCE/Wsnetwin` | 1 | 4 | 4 | 20% |
| `BASE/FONT` | 1 | 4 | 3 | 20% |
| `SOURCE/SETUP` | 1 | 7 | 10 | 12% |
| `SOURCE/dpnetwin` | 1 | 7 | 6 | 12% |
| `SOURCE/STRIP` | 1 | 7 | 0 | 12% |
| `SOURCE/REQUEST` | 1 | 9 | 4 | 10% |
| `BASE/WINDOW` | 1 | 12 | 2 | 8% |
| `SOURCE/X_CAMPGN` | 1 | 19 | 13 | 5% |

44 units diverge; 173 units show none.

## 7. "Nobody renames 258 functions" - what actually differs

The objection is right, and the data answers it: the divergence is **per variable, not
per function**, and it is one-directional.

### The compiler cannot be the cause - the images prove it themselves

Split the comparable functions by how many local slots they use:

| local slots | identical | slot-only | share differing |
|---|---:|---:|---:|
| 0-1 | 535 | 0 | 0% |
| 2 | 150 | 8 | 5% |
| 3-4 | 105 | 47 | 31% |
| 5+ | **65** | 203 | 76% |

**65 functions with five or more locals are byte-identical between 2.0 and 2.1, slot
displacements included.** Any global change to the hash, the table size, the walk order,
or the tie-break would have re-sorted those too - a five-local function survives a
changed rule with probability ~1/120. It is not the toolchain. The rising rate with local
count is simply the chance that *at least one* of a function's variables is spelled
differently.

### It is not a tie-break flip either

If the two builds had inserted same-bucket locals head-first vs tail-first, every
divergence would be a reversal of a *contiguous* run (same-bucket locals are adjacent in
the slot order). Of the 173 divergences that are pure permutations of the same slot set,
only 69 (40%) fit that shape, and displacements run up to **17 positions**. A local
cannot leave its own bucket without its spelling changing.

### Nothing outside the function moves its slots

Probe battery, one identical target function under ten different contexts, VC6 `/Od`:

| context | target frame |
|---|---|
| baseline | unchanged |
| preceding function declaring the *same* identifiers | unchanged |
| preceding function with colliding-bucket identifiers | unchanged |
| ten preceding functions | unchanged |
| unrelated `#define` before it | unchanged |
| file-scope globals added | unchanged |
| `goto` label added | unchanged |
| function made `static` | unchanged |
| **unused local added** | frame +4, all locals shift (top slot taken regardless of the unused local's name) |
| **parameter added** | frame +4 (spill slot) |

So there is no TU-state channel: a slot layout depends only on that function's own
referenced locals. The last two rows change the frame size, so they cannot hide inside a
slot-only divergence - those are excluded by construction.

### How much actually differs

Minimum number of locals that must change position, per divergent function:

| locals that must move | functions | share |
|---|---:|---:|
| 1 | 109 | 42% |
| 2 | 58 | 23% |
| 3 | 35 | 14% |
| 4-6 | 31 | 12% |
| 7+ | 14 | 5% |
| 0 (gap-only shift) | 11 | 4% |

**In 42% of cases exactly one local moves; in 65% at most two.** Summed over all 258
functions that is roughly 600 individual variable-level differences - spread across 44
source files and about a year of parallel development in the fork. That is ordinary
editing drift, not a renaming campaign. And 52 of them move by a single position, i.e.
one variable swapped with its neighbour.

### Direction

Across the 811 functions decidable in both the 1.0 and 2.1 censuses, the mainline
(`D:\heroes2` 1996 -> `F:\h2xsrc` 1997) shows **zero** changes; every divergence is on the
2.0 side. Whatever happened, it happened in the fork, not between 2.0 and 2.1.

*Inference, not measurement:* the most economical human story is a naming-convention or
cleanup pass in the expansion tree - it matches the fingerprint (bodies byte-identical,
one or two identifiers per touched function, confined to the files that team worked on).
We cannot confirm it: no build carries local-variable names. PoL 2.0's CodeView stream is
publics and thunks only, and only 2.0 has any `assert` expression strings, with no 2.1
counterpart to compare against.

## 8. Categorisation of all 258 slot-only functions

Permutation shapes across the 258:

| shape | count |
|---|---:|
| swap (single 2-cycle) | 64 |
| 3-cycle | 36 |
| 4-cycle | 15 |
| 5..12-cycle | 15 |
| multi-cycle | 41 |
| reshuffle touching unreferenced slots | 85 |
| 24-cycle (largest) | 1 |

Verdicts, where the PoL identifiers are byte-validated (92 functions with all-scalar
locals and a 100% PoL match):

| verdict | count | meaning |
|---|---:|---|
| identifiers-differ | **72** | the 2.1 order is not bucket-monotone in the 2.0 names, so no re-ordering of those identifiers can produce it - the spellings themselves differ |
| reorder-reachable | 20 | the 2.1 order *is* reachable by declaring the same names in another order |
| unproven | 166 | aggregate/array locals or no 100% PoL match - not decidable from source text alone |

The `1.0 witness` column marks the 112 functions where Heroes II 1.0 carries the same
layout as Gold 2.1, i.e. where 2.0 is demonstrably the deviation.

#### `BASE/Bzip` — 6 function(s)

| function | PoL rva | Gold 2.1 va | slots moved | permutation | 1.0 witness | verdict |
|---|---|---|---:|---|---|---|
| `?getRLEpair@@YIHPAU_iobuf@@@Z` | 0xd6f40 | 0x4d5480 | 2 | swap (2-cycle) | yes | - |
| `?loadAndRLEsource@@YIHPAU_iobuf@@@Z` | 0xd70a0 | 0x4d55e0 | 4 | multi-cycle 2+2 | - | - |
| `?unRLEandDump@@YIXPAU_iobuf@@H@Z` | 0xd7290 | 0x4d57d0 | 6 | 6-cycle | yes | - |
| `?uncompressStream@@YIHPAU_iobuf@@0@Z` | 0xd7710 | 0x4d5c10 | 10 | 10-cycle | yes | - |
| `?compress@@YIXPAD@Z` | 0xd7d60 | 0x4d61a0 | 3 | 3-cycle | yes | - |
| `?uncompress@@YIXPAD@Z` | 0xd7e50 | 0x4d6290 | 3 | 3-cycle | yes | - |

#### `BASE/FONT` — 1 function(s)

| function | PoL rva | Gold 2.1 va | slots moved | permutation | 1.0 witness | verdict |
|---|---|---|---:|---|---|---|
| `??0font@@QAE@K@Z` | 0xc6fd0 | 0x4cf680 | 2 | reshuffle (uses unreferenced slots) | yes | - |

#### `BASE/RESMGR` — 8 function(s)

| function | PoL rva | Gold 2.1 va | slots moved | permutation | 1.0 witness | verdict |
|---|---|---|---:|---|---|---|
| `?GetSample@resourceManager@@QAEPAVsample@@PAD@Z` | 0xc8570 | 0x4ccfd0 | 2 | swap (2-cycle) | yes | **names** |
| `?Expunge@resourceManager@@QAEXXZ` | 0xc87a0 | 0x4cd200 | 2 | swap (2-cycle) | - | - |
| `?LoadAggregateHeader@resourceManager@@QAEHPAD@Z` | 0xc8ab0 | 0x4cd510 | 3 | 3-cycle | - | - |
| `?PointToFile@resourceManager@@QAEXK@Z` | 0xc8c00 | 0x4cd660 | 4 | 4-cycle | - | **names** |
| `?GetFileSize@resourceManager@@QAEKK@Z` | 0xc8d20 | 0x4cd780 | 4 | multi-cycle 2+2 | - | **names** |
| `?ReadWord@resourceManager@@QAEFXZ` | 0xc8f70 | 0x4cd940 | 4 | multi-cycle 2+2 | - | - |
| `?ReadLong@resourceManager@@QAEJXZ` | 0xc9000 | 0x4cd9d0 | 4 | multi-cycle 2+2 | - | - |
| `?ReadBlock@resourceManager@@QAEXPACK@Z` | 0xc91b0 | 0x4cdb80 | 2 | swap (2-cycle) | - | - |

#### `BASE/WINDOW` — 1 function(s)

| function | PoL rva | Gold 2.1 va | slots moved | permutation | 1.0 witness | verdict |
|---|---|---|---:|---|---|---|
| `?MoveWindow@heroWindow@@QAEXHH@Z` | 0xcf950 | 0x4cc970 | 6 | 6-cycle | yes | **names** |

#### `EDITOR/mapcell` — 3 function(s)

| function | PoL rva | Gold 2.1 va | slots moved | permutation | 1.0 witness | verdict |
|---|---|---|---:|---|---|---|
| `?GetNewCellExtraIndex@fullMap@@QAEHXZ` | 0xb266 | 0x46f8d0 | 3 | 3-cycle | yes | **names** |
| `?Read@fullMap@@QAEXHH@Z` | 0xb7da | 0x46fe60 | 3 | 3-cycle | yes | order |
| `?ChangeTilesetIndex@fullMap@@QAEXPAVmapCell@@HHHHHH@Z` | 0xba6f | 0x470100 | 2 | reshuffle (uses unreferenced slots) | yes | - |

#### `SOURCE/ADVMGR` — 21 function(s)

| function | PoL rva | Gold 2.1 va | slots moved | permutation | 1.0 witness | verdict |
|---|---|---|---:|---|---|---|
| `?Open@advManager@@UAEHH@Z` | 0x5665f | 0x442fc0 | 4 | multi-cycle 2+2 | - | - |
| `?CheckSetEvilInterface@advManager@@QAEXHH@Z` | 0x57bdb | 0x444550 | 5 | reshuffle (uses unreferenced slots) | yes | - |
| `?ProcessSearch@advManager@@QAEHHH@Z` | 0x5a07c | 0x4469e0 | 5 | reshuffle (uses unreferenced slots) | - | - |
| `?UpdateHeroLocator@advManager@@QAEXHHH@Z` | 0x60465 | 0x44cd30 | 10 | reshuffle (uses unreferenced slots) | - | - |
| `?UpdBottomViewResMsg@advManager@@QAEHXZ` | 0x61716 | 0x44dfe0 | 4 | multi-cycle 2+2 | yes | - |
| `?DemobilizeCurrHero@advManager@@QAEXXZ` | 0x63f95 | 0x450880 | 2 | swap (2-cycle) | - | - |
| `?SetTownContext@advManager@@QAEXH@Z` | 0x64101 | 0x4509f0 | 2 | swap (2-cycle) | - | - |
| `?SetHeroContext@advManager@@QAEXHH@Z` | 0x64318 | 0x450c10 | 4 | 4-cycle | - | - |
| `?SaveGame@@YIHXZ` | 0x64e9f | 0x451750 | 6 | reshuffle (uses unreferenced slots) | - | - |
| `?DimensionDoorHandler@@YIHAAUtag_message@@@Z` | 0x65191 | 0x451a50 | 3 | 3-cycle | - | - |
| `?TeleportTo@advManager@@QAEXPAVhero@@HHHH@Z` | 0x6712a | 0x453a20 | 3 | reshuffle (uses unreferenced slots) | - | order |
| `?DimensionDoor@advManager@@QAEXXZ` | 0x67539 | 0x453e30 | 4 | multi-cycle 2+2 | - | **names** |
| `?ShowRoute@advManager@@QAEXHHH@Z` | 0x68247 | 0x454b50 | 14 | multi-cycle 7+4+3 | - | **names** |
| `?CheckScreenScroll@advManager@@QAEXXZ` | 0x68c5c | 0x455590 | 2 | swap (2-cycle) | - | **names** |
| `?SaveAdventureBorder@advManager@@QAEXXZ` | 0x69976 | 0x4563a0 | 3 | 3-cycle | yes | - |
| `?DrawAdventureBorder@advManager@@QAEXXZ` | 0x69abb | 0x4564f0 | 3 | 3-cycle | yes | - |
| `?ComputeAdvNetControl@@YIXXZ` | 0x69fc2 | 0x456a10 | 2 | swap (2-cycle) | yes | - |
| `?SystemOptions@advManager@@QAEXXZ` | 0x6b082 | 0x457b20 | 3 | reshuffle (uses unreferenced slots) | - | - |
| `?UpdateSystemOptions@@YIXH@Z` | 0x6b219 | 0x457cc0 | 5 | reshuffle (uses unreferenced slots) | yes | - |
| `?IsCrystalBallInEffect@advManager@@QAEHHHH@Z` | 0x6c241 | 0x458c60 | 3 | 3-cycle | - | **names** |
| `?StopOnTrigger@@YIEPAVmapCell@@@Z` | 0x6c318 | 0x458d40 | 2 | swap (2-cycle) | - | - |

#### `SOURCE/AI` — 14 function(s)

| function | PoL rva | Gold 2.1 va | slots moved | permutation | 1.0 witness | verdict |
|---|---|---|---:|---|---|---|
| `?GetShooterMask@combatManager@@QAEHH@Z` | 0xc26fa | 0x46bf70 | 2 | swap (2-cycle) | - | **names** |
| `?GetMirrorImageMask@combatManager@@QAEHH@Z` | 0xc282b | 0x46c0b0 | 2 | swap (2-cycle) | - | **names** |
| `?GetFlyerMask@combatManager@@QAEHH@Z` | 0xc28eb | 0x46c170 | 2 | swap (2-cycle) | - | **names** |
| `?GetAllMask@combatManager@@QAEHH@Z` | 0xc2a0a | 0x46c290 | 2 | swap (2-cycle) | - | **names** |
| `?GetWalkerMask@combatManager@@QAEHH@Z` | 0xc2aca | 0x46c350 | 2 | swap (2-cycle) | - | **names** |
| `?GetOutOfItMask@combatManager@@QAEHH@Z` | 0xc2c0b | 0x46c4a0 | 2 | swap (2-cycle) | - | **names** |
| `?GetTraitorMask@combatManager@@QAEHH@Z` | 0xc2cf4 | 0x46c590 | 2 | swap (2-cycle) | - | **names** |
| `?GetBestArmy@combatManager@@QAEHHH@Z` | 0xc2dca | 0x46c670 | 2 | swap (2-cycle) | - | **names** |
| `?GetWorstArmy@combatManager@@QAEHHH@Z` | 0xc2fbb | 0x46c870 | 2 | swap (2-cycle) | - | **names** |
| `?GetClosestArmy@combatManager@@QAEHPAVarmy@@HH@Z` | 0xc307d | 0x46c940 | 4 | multi-cycle 2+2 | - | - |
| `?GetStrength@combatManager@@QAEKHH@Z` | 0xc31ec | 0x46cab0 | 2 | swap (2-cycle) | - | **names** |
| `?AttemptAdjacentAttack@combatManager@@QAEHPAVarmy@@@Z` | 0xc3468 | 0x46cd40 | 5 | reshuffle (uses unreferenced slots) | - | - |
| `?WalkTowardArmyFront@combatManager@@QAEHPAVarmy@@HH@Z` | 0xc35ea | 0x46ced0 | 5 | 5-cycle | - | **names** |
| `?WalkTowardArmy@combatManager@@QAEHPAVarmy@@HH@Z` | 0xc382a | 0x46d110 | 6 | reshuffle (uses unreferenced slots) | - | **names** |

#### `SOURCE/ARMY` — 6 function(s)

| function | PoL rva | Gold 2.1 va | slots moved | permutation | 1.0 witness | verdict |
|---|---|---|---:|---|---|---|
| `?LoadResources@army@@QAEXXZ` | 0x4aec8 | 0x47b160 | 1 | reshuffle (uses unreferenced slots) | yes | order |
| `?Damage@army@@QAEHJH@Z` | 0x5012e | 0x480780 | 2 | swap (2-cycle) | - | - |
| `?PowEffect@army@@QAEXHHHH@Z` | 0x5036a | 0x4809c0 | 16 | multi-cycle 14+2 | - | - |
| `?SpellEffect@army@@QAEXHHH@Z` | 0x51b5b | 0x4821e0 | 6 | reshuffle (uses unreferenced slots) | yes | **names** |
| `?MoveAttack@army@@QAEXHH@Z` | 0x52ad9 | 0x483190 | 4 | multi-cycle 2+2 | - | - |
| `?OtherArmyAdjacent@army@@QAEHHH@Z` | 0x53fa6 | 0x4846b0 | 6 | multi-cycle 3+3 | - | - |

#### `SOURCE/ARMYGRP` — 3 function(s)

| function | PoL rva | Gold 2.1 va | slots moved | permutation | 1.0 witness | verdict |
|---|---|---|---:|---|---|---|
| `?GetMorale@armyGroup@@QAEHPAVhero@@PAVtown@@PAV1@@Z` | 0x8c17a | 0x46eed0 | 5 | 5-cycle | - | - |
| `?IsHomogeneous@armyGroup@@QAEHH@Z` | 0x8c44b | 0x46f1b0 | 2 | swap (2-cycle) | yes | - |
| `?DamageGroup@armyGroup@@QAEXM@Z` | 0x8c7d2 | 0x46f560 | 5 | multi-cycle 3+2 | yes | order |

#### `SOURCE/CMBTMGR` — 11 function(s)

| function | PoL rva | Gold 2.1 va | slots moved | permutation | 1.0 witness | verdict |
|---|---|---|---:|---|---|---|
| `?Open@combatManager@@UAEHH@Z` | 0x90aa0 | 0x4751c0 | 3 | 3-cycle | - | - |
| `?Close@combatManager@@UAEXXZ` | 0x90edf | 0x475600 | 2 | swap (2-cycle) | - | - |
| `?GenerateMap@combatManager@@QAEXXZ` | 0x91641 | 0x475d70 | 3 | reshuffle (uses unreferenced slots) | - | order |
| `?LoadArmies@combatManager@@QAEXXZ` | 0x922e5 | 0x476a50 | 3 | 3-cycle | - | - |
| `?GetGridIndex@combatManager@@QAEHHH@Z` | 0x9272d | 0x476ea0 | 7 | 7-cycle | - | **names** |
| `?CheckApplyGoodMorale@combatManager@@QAEXHH@Z` | 0x9290f | 0x477090 | 3 | 3-cycle | - | - |
| `?CheckApplyBadMorale@combatManager@@QAEHHH@Z` | 0x92afa | 0x477280 | 3 | 3-cycle | - | - |
| `?GetNextArmy@combatManager@@QAEHH@Z` | 0x92cc7 | 0x477450 | 7 | 7-cycle | - | - |
| `?IsWinner@combatManager@@QAEHH@Z` | 0x93049 | 0x4777e0 | 2 | swap (2-cycle) | - | **names** |
| `?MakeCreaturesVanish@combatManager@@QAEXXZ` | 0x94fb9 | 0x4796f0 | 6 | 6-cycle | - | - |
| `?LowerDoor@combatManager@@QAEXXZ` | 0x9525a | 0x4799a0 | 3 | 3-cycle | yes | - |

#### `SOURCE/COMMAND` — 15 function(s)

| function | PoL rva | Gold 2.1 va | slots moved | permutation | 1.0 witness | verdict |
|---|---|---|---:|---|---|---|
| `?Main@combatManager@@UAEHAAUtag_message@@@Z` | 0x2a6d0 | 0x43b090 | 2 | swap (2-cycle) | - | - |
| `?GetPointer@combatManager@@QAEHHH@Z` | 0x2ba9b | 0x43c470 | 2 | reshuffle (uses unreferenced slots) | yes | - |
| `?GetCommand@combatManager@@QAEHH@Z` | 0x2c8ff | 0x43d2f0 | 6 | multi-cycle 4+2 | - | - |
| `?RightClick@combatManager@@QAEHH@Z` | 0x2ce19 | 0x43d810 | 2 | swap (2-cycle) | - | order |
| `?WinCombatHandler@@YIHAAUtag_message@@@Z` | 0x2d472 | 0x43de80 | 3 | reshuffle (uses unreferenced slots) | - | - |
| `?ShowWinLoseArtifact@combatManager@@QAEXPAVheroWindow@@H@Z` | 0x2dafd | 0x43e510 | 8 | reshuffle (uses unreferenced slots) | - | - |
| `?ShowSkeletons@combatManager@@QAEXPAVheroWindow@@@Z` | 0x2dd97 | 0x43e7b0 | 7 | reshuffle (uses unreferenced slots) | - | - |
| `?ShowEagleEyeSpell@combatManager@@QAEXPAVheroWindow@@@Z` | 0x2dfc9 | 0x43e9f0 | 10 | reshuffle (uses unreferenced slots) | - | - |
| `?DoVictory@combatManager@@QAEXH@Z` | 0x2ec8b | 0x43f6e0 | 15 | reshuffle (uses unreferenced slots) | - | - |
| `?DoLoseWindow@combatManager@@QAEXXZ` | 0x2f834 | 0x440290 | 7 | reshuffle (uses unreferenced slots) | - | - |
| `?DoSurrender@combatManager@@QAEHXZ` | 0x2fbf0 | 0x440650 | 9 | reshuffle (uses unreferenced slots) | - | - |
| `?ResetMouse@combatManager@@QAEXXZ` | 0x3045f | 0x440ed0 | 7 | reshuffle (uses unreferenced slots) | - | - |
| `?ResetCyclingCreatures@combatManager@@QAEXXZ` | 0x30b91 | 0x441610 | 4 | reshuffle (uses unreferenced slots) | - | - |
| `?ResetCycleTimers@combatManager@@QAEXXZ` | 0x30dc8 | 0x441850 | 3 | 3-cycle | - | - |
| `?AddArmy@combatManager@@QAEXHHHHHH@Z` | 0x31a0b | 0x4424b0 | 4 | 4-cycle | - | **names** |

#### `SOURCE/CURSOR` — 9 function(s)

| function | PoL rva | Gold 2.1 va | slots moved | permutation | 1.0 witness | verdict |
|---|---|---|---:|---|---|---|
| `?ProcessIncomingGroupMapChange@advManager@@QAEXPAD@Z` | 0x10a5b | 0x4c2e60 | 2 | swap (2-cycle) | yes | - |
| `?UnwindMapChangeQueue@advManager@@QAEXHH@Z` | 0x10b9e | 0x4c2fb0 | 6 | 6-cycle | yes | **names** |
| `?StartCursor@advManager@@QAEXH@Z` | 0xd5e0 | 0x4bf980 | 4 | 4-cycle | - | **names** |
| `?DrawCursor@advManager@@QAEXXZ` | 0xd834 | 0x4bfbe0 | 2 | swap (2-cycle) | - | order |
| `?DrawCursorShadow@advManager@@QAEXXZ` | 0xde2c | 0x4c01e0 | 2 | swap (2-cycle) | - | **names** |
| `?TurnTo@advManager@@QAEXH@Z` | 0xe21d | 0x4c05e0 | 2 | swap (2-cycle) | - | **names** |
| `?GetMoveShowIt@advManager@@QAEHPAVhero@@H@Z` | 0xe473 | 0x4c0840 | 2 | swap (2-cycle) | yes | **names** |
| `?ValidMoveWithEvent@advManager@@QAEHPAVhero@@H@Z` | 0xf8c7 | 0x4c1cb0 | 4 | 4-cycle | - | **names** |
| `?ValidMove@advManager@@QAEHHH@Z` | 0xfa15 | 0x4c1e00 | 12 | 12-cycle | - | **names** |

#### `SOURCE/Campaign` — 1 function(s)

| function | PoL rva | Gold 2.1 va | slots moved | permutation | 1.0 witness | verdict |
|---|---|---|---:|---|---|---|
| `?ShowCampaignInfo@game@@QAEXHH@Z` | 0x47fb6 | 0x4a8140 | 6 | reshuffle (uses unreferenced slots) | - | - |

#### `SOURCE/DRAWING` — 3 function(s)

| function | PoL rva | Gold 2.1 va | slots moved | permutation | 1.0 witness | verdict |
|---|---|---|---:|---|---|---|
| `?SetupGridForArmy@combatManager@@QAEXPAVarmy@@@Z` | 0x340e | 0x4c3dd0 | 4 | multi-cycle 2+2 | - | - |
| `?UpdateGrid@combatManager@@QAEHHH@Z` | 0x3621 | 0x4c3ff0 | 9 | multi-cycle 6+3 | - | **names** |
| `?UpdateMouseGrid@combatManager@@QAEXHH@Z` | 0x3f80 | 0x4c4960 | 7 | multi-cycle 4+3 | - | **names** |

#### `SOURCE/EVENTS` — 12 function(s)

| function | PoL rva | Gold 2.1 va | slots moved | permutation | 1.0 witness | verdict |
|---|---|---|---:|---|---|---|
| `?BarrierEvent@advManager@@QAEHPAVmapCell@@PAVhero@@@Z` | 0xaea92 | 0x48c4c0 | 5 | reshuffle (uses unreferenced slots) | - | - |
| `?StrEqNoCase@@YICPAD0@Z` | 0xaebc1 | 0x48c5f0 | 5 | multi-cycle 3+2 | - | **names** |
| `?PasswordEvent@advManager@@QAEXPAVmapCell@@PAVhero@@@Z` | 0xaec5a | 0x48c690 | 3 | 3-cycle | - | - |
| `?ExpansionRecruitEvent@advManager@@QAEXPAVhero@@HPAF@Z` | 0xaf5c7 | 0x48d030 | 1 | reshuffle (uses unreferenced slots) | - | - |
| `?JailEvent@advManager@@QAEXPAVmapCell@@PAVhero@@HH@Z` | 0xaf652 | 0x48d0c0 | 3 | 3-cycle | - | - |
| `?TownEvent@advManager@@QAEXPAVmapCell@@HH@Z` | 0xaf87c | 0x48d2f0 | 2 | swap (2-cycle) | - | - |
| `?RecruitEvent@advManager@@QAEXPAVhero@@HPAVmapCell@@@Z` | 0xb022e | 0x48dcd0 | 2 | reshuffle (uses unreferenced slots) | yes | - |
| `?DoWhirlpool@advManager@@QAEXPAVhero@@@Z` | 0xb1bcf | 0x48f6b0 | 3 | reshuffle (uses unreferenced slots) | yes | - |
| `?FizzleCenter@advManager@@QAEXH@Z` | 0xb1d01 | 0x48f7f0 | 3 | 3-cycle | yes | - |
| `?BarrierAIEvent@advManager@@QAEHPAVmapCell@@PAVhero@@@Z` | 0xb4883 | 0x492310 | 1 | reshuffle (uses unreferenced slots) | - | - |
| `?DoNetCombat@advManager@@QAEHPAD@Z` | 0xb5c40 | 0x4936e0 | 10 | reshuffle (uses unreferenced slots) | yes | - |
| `?ReceiveHeroTownData@advManager@@QAEXPADPAH11PAPAVhero@@PAPAVarmyGroup@@PAPAVtown@@23111PAC55@Z` | 0xb67cd | 0x494270 | 11 | multi-cycle 6+3+2 | - | - |

#### `SOURCE/FLY` — 1 function(s)

| function | PoL rva | Gold 2.1 va | slots moved | permutation | 1.0 witness | verdict |
|---|---|---|---:|---|---|---|
| `?ValidFlight@army@@QAEHHH@Z` | 0xa5b95 | 0x4012c0 | 5 | reshuffle (uses unreferenced slots) | - | - |

#### `SOURCE/GAME` — 28 function(s)

| function | PoL rva | Gold 2.1 va | slots moved | permutation | 1.0 witness | verdict |
|---|---|---|---:|---|---|---|
| `?NextHero@playerData@@QAEHH@Z` | 0x70d1a | 0x418ce0 | 2 | swap (2-cycle) | yes | **names** |
| `?GetNumObelisks@@YIHH@Z` | 0x70eac | 0x418e80 | 2 | swap (2-cycle) | - | **names** |
| `?NumOfGivenArtifact@playerData@@QAEHH@Z` | 0x70fda | 0x418fc0 | 2 | swap (2-cycle) | - | order |
| `?MineTypesOwned@game@@QAEHHH@Z` | 0x71071 | 0x419060 | 2 | swap (2-cycle) | yes | **names** |
| `?ComputeUALoc@@YIXH@Z` | 0x710f3 | 0x4190f0 | 3 | 3-cycle | - | - |
| `?IsMobile@game@@QAEHH@Z` | 0x717ac | 0x419790 | 2 | swap (2-cycle) | - | - |
| `?CreateBoat@game@@QAEHHHH@Z` | 0x7187f | 0x419870 | 2 | swap (2-cycle) | - | order |
| `?GetNewHeroId@game@@QAEHHHH@Z` | 0x71a6d | 0x419a70 | 3 | 3-cycle | - | **names** |
| `?GenerateStandardFileName@@YIXPAD0@Z` | 0x71d89 | 0x419db0 | 4 | 4-cycle | yes | - |
| `?RandomizeBarrier@game@@QAEXPAVmapCell@@@Z` | 0x78aea | 0x420a10 | 2 | swap (2-cycle) | - | order |
| `?LoadMap@game@@QAEHPAD@Z` | 0x78b72 | 0x420ab0 | 5 | reshuffle (uses unreferenced slots) | - | - |
| `?ClaimTown@game@@QAEXHHH@Z` | 0x78fea | 0x420f30 | 2 | swap (2-cycle) | yes | **names** |
| `?ComputeDailyGold@game@@QAEHH@Z` | 0x7c32f | 0x4243f0 | 3 | 3-cycle | - | **names** |
| `?PerDay@game@@QAEXXZ` | 0x7c761 | 0x424830 | 7 | reshuffle (uses unreferenced slots) | - | order |
| `?PerMonth@game@@QAEXXZ` | 0x7ec44 | 0x426d20 | 3 | 3-cycle | yes | **names** |
| `?RandomizeTown@game@@QAEXHHH@Z` | 0x7f42f | 0x427520 | 4 | reshuffle (uses unreferenced slots) | yes | - |
| `?InitRandomArtifacts@game@@QAEXXZ` | 0x7fc0a | 0x427d10 | 1 | reshuffle (uses unreferenced slots) | - | - |
| `?GiveArmy@game@@QAEXPAVarmyGroup@@HHH@Z` | 0x80e6c | 0x428fe0 | 1 | reshuffle (uses unreferenced slots) | yes | order |
| `?SetupAdjacentMons@game@@QAEXXZ` | 0x8111f | 0x4292b0 | 4 | multi-cycle 2+2 | yes | **names** |
| `?ProcessMapExtra@game@@QAEXXZ` | 0x819b2 | 0x429b90 | 4 | 4-cycle | - | - |
| `?ProcessOnMapHeroes@game@@QAEXXZ` | 0x82547 | 0x42a730 | 13 | multi-cycle 5+2+2+2+2 | - | - |
| `?CheckHeroConsistency@game@@QAEXXZ` | 0x82cbb | 0x42aeb0 | 4 | 4-cycle | - | **names** |
| `?CreateJoinFile@@YIXPAD00@Z` | 0x85107 | 0x42d5b0 | 11 | multi-cycle 6+3+2 | yes | - |
| `?GetMapEvent@@YIPAUEventExtra@@HH@Z` | 0x85d2e | 0x42e250 | 2 | swap (2-cycle) | - | - |
| `?CalcFileCRC@@YIHPAD@Z` | 0x86296 | 0x42e7d0 | 4 | 4-cycle | yes | - |
| `?CompressTest2@@YIXXZ` | 0x86341 | 0x42e880 | 9 | multi-cycle 6+3 | yes | **names** |
| `?CompressTest@@YIXXZ` | 0x86494 | 0x42e9e0 | 10 | reshuffle (uses unreferenced slots) | yes | - |
| `?CompressTest3@@YIXXZ` | 0x86652 | 0x42eba0 | 2 | reshuffle (uses unreferenced slots) | yes | - |

#### `SOURCE/HERO` — 10 function(s)

| function | PoL rva | Gold 2.1 va | slots moved | permutation | 1.0 witness | verdict |
|---|---|---|---:|---|---|---|
| `?GetNumSpells@hero@@QAEHH@Z` | 0x6c931 | 0x494fd0 | 3 | 3-cycle | - | - |
| `?HeroScreenUpdate@hero@@QAEXXZ` | 0x6cb33 | 0x4951f0 | 5 | reshuffle (uses unreferenced slots) | yes | - |
| `?UpdateArmies@hero@@QAEXXZ` | 0x6cbdb | 0x4952a0 | 5 | reshuffle (uses unreferenced slots) | yes | - |
| `?Deallocate@hero@@QAEXH@Z` | 0x6cee8 | 0x4955d0 | 4 | 4-cycle | - | **names** |
| `?GetExperience@hero@@QAEHH@Z` | 0x6d46f | 0x495b60 | 3 | 3-cycle | yes | **names** |
| `?GetLevel@hero@@QAEHH@Z` | 0x6d50d | 0x495c00 | 3 | 3-cycle | yes | **names** |
| `?SetupHeroView@@YIXXZ` | 0x6f56c | 0x497b90 | 11 | reshuffle (uses unreferenced slots) | - | - |
| `?DoHeroSplit@@YIXHH@Z` | 0x6ff31 | 0x498560 | 5 | reshuffle (uses unreferenced slots) | yes | - |
| `?GetSSLevel@hero@@QAECH@Z` | 0x705c2 | 0x498c20 | 2 | swap (2-cycle) | - | order |
| `?DoSSLevelDialog@hero@@QAEXHH@Z` | 0x70685 | 0x498cf0 | 2 | swap (2-cycle) | - | **names** |

#### `SOURCE/KB` — 10 function(s)

| function | PoL rva | Gold 2.1 va | slots moved | permutation | 1.0 witness | verdict |
|---|---|---|---:|---|---|---|
| `?RecruitHeroHandler@@YIHAAUtag_message@@@Z` | 0x993e0 | 0x431d10 | 4 | reshuffle (uses unreferenced slots) | yes | - |
| `?CanBuild@@YIHPAVtown@@H@Z` | 0x99a6c | 0x4323c0 | 3 | reshuffle (uses unreferenced slots) | - | **names** |
| `?CanBuy@@YIHPAVtown@@H@Z` | 0x99d21 | 0x432680 | 2 | swap (2-cycle) | yes | - |
| `?QuickViewWait@@YIXXZ` | 0x9c07c | 0x434ac0 | 2 | reshuffle (uses unreferenced slots) | yes | - |
| `?FileError@@YIXPAD@Z` | 0x9e306 | 0x436df0 | 2 | swap (2-cycle) | yes | - |
| `?SmackFade@@YIXPAE0@Z` | 0x9e3a8 | 0x436ea0 | 9 | 9-cycle | yes | - |
| `?CongratsWait@@YIXXZ` | 0x9e900 | 0x437440 | 3 | reshuffle (uses unreferenced slots) | yes | - |
| `?SetupDynamicWindow@@YIXHHHHHHHPAH00000PAPAVheroWindow@@H@Z` | 0x9fa70 | 0x438610 | 24 | 24-cycle | yes | - |
| `?TestDynamicWindow@@YIXHH@Z` | 0xa012c | 0x438cd0 | 7 | 7-cycle | yes | - |
| `?SetWinText@@YIXPAVheroWindow@@H@Z` | 0xa0c76 | 0x439830 | 5 | reshuffle (uses unreferenced slots) | yes | - |

#### `SOURCE/Modem` — 2 function(s)

| function | PoL rva | Gold 2.1 va | slots moved | permutation | 1.0 witness | verdict |
|---|---|---|---:|---|---|---|
| `?Connect@@YIXXZ` | 0xcfec | 0x4645a0 | 2 | reshuffle (uses unreferenced slots) | yes | - |
| `?WriteModemPacket@@YIXPADH@Z` | 0xd4df | 0x464ab0 | 1 | reshuffle (uses unreferenced slots) | yes | - |

#### `SOURCE/Newgame` — 8 function(s)

| function | PoL rva | Gold 2.1 va | slots moved | permutation | 1.0 witness | verdict |
|---|---|---|---:|---|---|---|
| `?GetMap@game@@QAEXXZ` | 0xb6f40 | 0x45a7a0 | 3 | reshuffle (uses unreferenced slots) | - | - |
| `?InitNewGame@game@@QAEXPAUSMapHeader@@@Z` | 0xb718c | 0x45aa00 | 5 | 5-cycle | yes | - |
| `?NewGame@game@@QAEHXZ` | 0xb769e | 0x45af30 | 9 | reshuffle (uses unreferenced slots) | - | - |
| `?InitNewGameWindow@game@@QAEXXZ` | 0xb8259 | 0x45bb00 | 9 | multi-cycle 4+3+2 | - | - |
| `?UpdateNewGameWindow@game@@QAEXXZ` | 0xb88d6 | 0x45c180 | 6 | reshuffle (uses unreferenced slots) | - | - |
| `?ShowScenInfo@game@@QAEXXZ` | 0xba39c | 0x45dc50 | 18 | reshuffle (uses unreferenced slots) | yes | - |
| `?GetLossConditionText@game@@QAEXPAD@Z` | 0xbaf0d | 0x45e7d0 | 4 | 4-cycle | - | **names** |
| `?GetSideDesc@game@@QAEHPADHH@Z` | 0xbb3cf | 0x45eca0 | 7 | reshuffle (uses unreferenced slots) | yes | - |

#### `SOURCE/Overview` — 1 function(s)

| function | PoL rva | Gold 2.1 va | slots moved | permutation | 1.0 witness | verdict |
|---|---|---|---:|---|---|---|
| `?SetupResources@game@@QAEXXZ` | 0x9e05 | 0x4624f0 | 5 | reshuffle (uses unreferenced slots) | yes | - |

#### `SOURCE/PATH` — 7 function(s)

| function | PoL rva | Gold 2.1 va | slots moved | permutation | 1.0 witness | verdict |
|---|---|---|---:|---|---|---|
| `?FindPath@army@@QAEHHHHHH@Z` | 0xbdbf0 | 0x405db0 | 2 | swap (2-cycle) | yes | **names** |
| `?ValidPath@army@@QAEHHH@Z` | 0xbdd3a | 0x405f00 | 1 | reshuffle (uses unreferenced slots) | yes | order |
| `?GetMoveMask@army@@QAEHH@Z` | 0xbddd4 | 0x405fa0 | 3 | 3-cycle | yes | - |
| `?GetAttackMask@army@@QAEHHHH@Z` | 0xbde41 | 0x406010 | 5 | 5-cycle | yes | - |
| `?ValidMove@army@@QAEHHH@Z` | 0xbdf21 | 0x406100 | 4 | multi-cycle 2+2 | - | **names** |
| `?ValidRange@army@@QAEHH@Z` | 0xbe59d | 0x4067a0 | 2 | swap (2-cycle) | yes | - |
| `?GetBestDirection@army@@QAEHHHH@Z` | 0xbea3f | 0x406c50 | 6 | multi-cycle 4+2 | yes | **names** |

#### `SOURCE/PHILAI` — 19 function(s)

| function | PoL rva | Gold 2.1 va | slots moved | permutation | 1.0 witness | verdict |
|---|---|---|---:|---|---|---|
| `?CheckDoMain@@YIXHH@Z` | 0x379d0 | 0x4aa7b0 | 3 | 3-cycle | - | - |
| `?CheckForCreatureUpgrades@philAI@@QAEXXZ` | 0x37c61 | 0x4aaa50 | 11 | multi-cycle 4+4+3 | - | - |
| `?CheckBuyStuff@philAI@@QAEXXZ` | 0x37fdf | 0x4aadd0 | 7 | 7-cycle | yes | - |
| `?GoodAdjacent@philAI@@QAEHPAH@Z` | 0x3849d | 0x4ab290 | 9 | multi-cycle 5+2+2 | - | - |
| `?DoAnywhereDDoorTownGate@philAI@@QAEHH@Z` | 0x38fb0 | 0x4abd20 | 12 | multi-cycle 9+3 | - | - |
| `?SetupRelativeHeroStrengths@philAI@@QAEXXZ` | 0x39501 | 0x4ac280 | 3 | 3-cycle | - | **names** |
| `?DetermineHeroToMove@philAI@@QAEPAVhero@@H@Z` | 0x3b748 | 0x4ae470 | 2 | swap (2-cycle) | - | - |
| `?GetTurnAttentionValue@philAI@@QAEXH@Z` | 0x3e6b0 | 0x4b12f0 | 2 | swap (2-cycle) | yes | order |
| `?TurnsToBuy@philAI@@QAEMQAH@Z` | 0x3e848 | 0x4b14a0 | 2 | swap (2-cycle) | yes | **names** |
| `?FutureDeflator@philAI@@QAEMQAH@Z` | 0x3fe81 | 0x4b2a50 | 2 | swap (2-cycle) | yes | **names** |
| `?ChooseEvaluateBattle@philAI@@QAEXPAVarmyGroup@@PAVhero@@01HHHAAH2@Z` | 0x425b0 | 0x4b5140 | 6 | reshuffle (uses unreferenced slots) | yes | - |
| `?ChooseToFightForArtifact@philAI@@QAEHHHH@Z` | 0x42677 | 0x4b5210 | 8 | multi-cycle 5+3 | yes | order |
| `?BuildHero@philAI@@QAEXPAVtown@@H@Z` | 0x428b8 | 0x4b5470 | 3 | 3-cycle | - | **names** |
| `?CombatMonsterEvent@philAI@@QAEHPAVhero@@HPAHPAVmapCell@@@Z` | 0x43007 | 0x4b5bc0 | 2 | swap (2-cycle) | yes | order |
| `?TownEvent@philAI@@QAEXPAVmapCell@@PAVhero@@HH@Z` | 0x43980 | 0x4b6560 | 5 | multi-cycle 3+2 | - | - |
| `?ComputeUpgradeValue@philAI@@QAEHHH@Z` | 0x43ba7 | 0x4b6790 | 2 | swap (2-cycle) | yes | order |
| `?EvaluateMineEvent@philAI@@QAEHHHHPAH@Z` | 0x4661e | 0x4b91e0 | 9 | multi-cycle 5+4 | - | **names** |
| `?EvaluateHeroEvent@philAI@@QAEHHHHHPAH@Z` | 0x46c66 | 0x4b9820 | 10 | reshuffle (uses unreferenced slots) | - | - |
| `?EvaluateTownEvent@philAI@@QAEHHHHHPAH@Z` | 0x47211 | 0x4b9dd0 | 8 | multi-cycle 5+3 | - | - |

#### `SOURCE/RECRUIT` — 2 function(s)

| function | PoL rva | Gold 2.1 va | slots moved | permutation | 1.0 witness | verdict |
|---|---|---|---:|---|---|---|
| `?SetupRecruitWin@@YIXPAVheroWindow@@HHHHH@Z` | 0x8b310 | 0x4080f0 | 6 | reshuffle (uses unreferenced slots) | yes | - |
| `?Main@recruitUnit@@UAEHAAUtag_message@@@Z` | 0x8b8f0 | 0x4086f0 | 2 | swap (2-cycle) | yes | **names** |

#### `SOURCE/REMOTE` — 3 function(s)

| function | PoL rva | Gold 2.1 va | slots moved | permutation | 1.0 witness | verdict |
|---|---|---|---:|---|---|---|
| `?calc_crc_long@@YIHPAEH@Z` | 0xa3967 | 0x46dc60 | 3 | 3-cycle | yes | **names** |
| `?PollRemote@@YIXXZ` | 0xa41ec | 0x46e500 | 2 | swap (2-cycle) | - | - |
| `?TransmitAndWait@@YIHPADHHCCPAPAD@Z` | 0xa48e0 | 0x46ec00 | 4 | 4-cycle | yes | **names** |

#### `SOURCE/REQUEST` — 1 function(s)

| function | PoL rva | Gold 2.1 va | slots moved | permutation | 1.0 witness | verdict |
|---|---|---|---:|---|---|---|
| `?Open@fileRequester@@UAEHH@Z` | 0x8d5e1 | 0x40c100 | 6 | reshuffle (uses unreferenced slots) | yes | - |

#### `SOURCE/SETUP` — 1 function(s)

| function | PoL rva | Gold 2.1 va | slots moved | permutation | 1.0 witness | verdict |
|---|---|---|---:|---|---|---|
| `?PickLoadGame@game@@QAEHXZ` | 0x123cc | 0x4bb840 | 3 | reshuffle (uses unreferenced slots) | - | - |

#### `SOURCE/SPELLAI` — 2 function(s)

| function | PoL rva | Gold 2.1 va | slots moved | permutation | 1.0 witness | verdict |
|---|---|---|---:|---|---|---|
| `?DoSpellAI@combatManager@@QAEHHH@Z` | 0x867c0 | 0x465a70 | 6 | multi-cycle 4+2 | - | - |
| `?EffectSpellResurrect@combatManager@@QAEXPAHHH@Z` | 0x88be2 | 0x467ed0 | 4 | reshuffle (uses unreferenced slots) | - | - |

#### `SOURCE/SPELLS` — 7 function(s)

| function | PoL rva | Gold 2.1 va | slots moved | permutation | 1.0 witness | verdict |
|---|---|---|---:|---|---|---|
| `?FindResurrectArmyIndex@combatManager@@QAEHHHH@Z` | 0x20d96 | 0x49dc70 | 2 | swap (2-cycle) | - | - |
| `?ElementalStorm@combatManager@@QAEXXZ` | 0x2414e | 0x4a1070 | 8 | multi-cycle 4+4 | - | - |
| `?GetNextChainLightningTarget@combatManager@@QAEHPAVarmy@@H@Z` | 0x266ce | 0x4a3610 | 9 | multi-cycle 4+3+2 | - | - |
| `?ChainLightning@combatManager@@QAEXHH@Z` | 0x2685a | 0x4a37a0 | 16 | reshuffle (uses unreferenced slots) | - | **names** |
| `?VaporizeCreature@combatManager@@QAEXHH@Z` | 0x26bbb | 0x4a3b10 | 7 | reshuffle (uses unreferenced slots) | - | - |
| `?DoLuck@combatManager@@QAEXHH@Z` | 0x28b69 | 0x4a5c10 | 5 | multi-cycle 3+2 | - | **names** |
| `?Resurrect@combatManager@@QAEXHHH@Z` | 0x29089 | 0x4a6120 | 11 | reshuffle (uses unreferenced slots) | - | **names** |

#### `SOURCE/STRIP` — 1 function(s)

| function | PoL rva | Gold 2.1 va | slots moved | permutation | 1.0 witness | verdict |
|---|---|---|---:|---|---|---|
| `?Update@bankBox@@QAEXH@Z` | 0x32b2d | 0x409780 | 5 | reshuffle (uses unreferenced slots) | yes | - |

#### `SOURCE/SWAPMGR` — 6 function(s)

| function | PoL rva | Gold 2.1 va | slots moved | permutation | 1.0 witness | verdict |
|---|---|---|---:|---|---|---|
| `?Open@swapManager@@UAEHH@Z` | 0x54502 | 0x4bdb00 | 6 | reshuffle (uses unreferenced slots) | - | - |
| `?DrawSelector@swapManager@@QAEXXZ` | 0x549eb | 0x4bdff0 | 8 | multi-cycle 6+2 | yes | - |
| `?SwapArtifacts@swapManager@@QAEXXZ` | 0x55776 | 0x4bed90 | 6 | reshuffle (uses unreferenced slots) | - | - |
| `?SwapMons@swapManager@@QAEXXZ` | 0x559cb | 0x4beff0 | 2 | swap (2-cycle) | yes | order |
| `?Update@swapManager@@QAEXXZ` | 0x55b42 | 0x4bf170 | 5 | reshuffle (uses unreferenced slots) | - | - |
| `?SplitMons@swapManager@@QAEXXZ` | 0x55fbd | 0x4bf5f0 | 10 | reshuffle (uses unreferenced slots) | yes | **names** |

#### `SOURCE/TOWN` — 2 function(s)

| function | PoL rva | Gold 2.1 va | slots moved | permutation | 1.0 witness | verdict |
|---|---|---|---:|---|---|---|
| `?GiveSpells@town@@QAEXPAVhero@@@Z` | 0x32cb9 | 0x469880 | 3 | 3-cycle | - | **names** |
| `?Deallocate@town@@QAEXXZ` | 0x32f54 | 0x469b30 | 2 | swap (2-cycle) | yes | **names** |

#### `SOURCE/TOWNMGR` — 7 function(s)

| function | PoL rva | Gold 2.1 va | slots moved | permutation | 1.0 witness | verdict |
|---|---|---|---:|---|---|---|
| `??0townObject@@QAE@HHPAD@Z` | 0x13900 | 0x4106a0 | 6 | reshuffle (uses unreferenced slots) | yes | - |
| `?SetupTown@townManager@@QAEXXZ` | 0x1436f | 0x411140 | 5 | reshuffle (uses unreferenced slots) | - | - |
| `?SplitArmy@townManager@@QAEXXZ` | 0x1771d | 0x414520 | 5 | reshuffle (uses unreferenced slots) | yes | order |
| `?BuildObj@townManager@@QAEXH@Z` | 0x18bd2 | 0x4158d0 | 5 | reshuffle (uses unreferenced slots) | yes | - |
| `?SetupMage@townManager@@QAEXPAVheroWindow@@@Z` | 0x18fbb | 0x415cc0 | 17 | reshuffle (uses unreferenced slots) | yes | **names** |
| `?MageGuildHandler@@YIHAAUtag_message@@@Z` | 0x19393 | 0x4160a0 | 8 | multi-cycle 5+3 | yes | - |
| `?SplitArmyHandler@@YIHAAUtag_message@@@Z` | 0x19e8c | 0x416bc0 | 4 | 4-cycle | yes | - |

#### `SOURCE/VIEW` — 3 function(s)

| function | PoL rva | Gold 2.1 va | slots moved | permutation | 1.0 witness | verdict |
|---|---|---|---:|---|---|---|
| `?ViewGeneral@combatManager@@QAEHHHH@Z` | 0xbd60 | 0x40faf0 | 22 | reshuffle (uses unreferenced slots) | - | - |
| `?HandleViewGeneral@@YIHAAUtag_message@@@Z` | 0xc433 | 0x4101d0 | 15 | multi-cycle 13+2 | yes | - |
| `?ViewArmy@combatManager@@QAEXPAVarmy@@H@Z` | 0xc784 | 0x410530 | 8 | multi-cycle 6+2 | - | - |

#### `SOURCE/Wsnetwin` — 1 function(s)

| function | PoL rva | Gold 2.1 va | slots moved | permutation | 1.0 witness | verdict |
|---|---|---|---:|---|---|---|
| `?wsnet_init@@YIFXZ` | 0x68b0 | 0x45ef50 | 3 | 3-cycle | - | - |

#### `SOURCE/X_CAMPGN` — 1 function(s)

| function | PoL rva | Gold 2.1 va | slots moved | permutation | 1.0 witness | verdict |
|---|---|---|---:|---|---|---|
| `?ShowInfo@ExpCampaign@@QAEXHH@Z` | 0xbc00e | 0x470df0 | 3 | 3-cycle | - | - |

#### `SOURCE/comwin` — 3 function(s)

| function | PoL rva | Gold 2.1 va | slots moved | permutation | 1.0 witness | verdict |
|---|---|---|---:|---|---|---|
| `?com_rcv@@YIFFGPAX@Z` | 0x8af7a | 0x4bd0c0 | 5 | reshuffle (uses unreferenced slots) | yes | - |
| `?com_snd@@YIFFGGPAXH@Z` | 0x8b057 | 0x4bd1a0 | 2 | swap (2-cycle) | yes | - |
| `?comm_wrt_task@@YIXXZ` | 0x8b21d | 0x4bd380 | 5 | 5-cycle | yes | - |

#### `SOURCE/dpnetwin` — 1 function(s)

| function | PoL rva | Gold 2.1 va | slots moved | permutation | 1.0 witness | verdict |
|---|---|---|---:|---|---|---|
| `?dpSendMessage@@YIXHEGPAX@Z` | 0x1f3a4 | 0x404c60 | 2 | swap (2-cycle) | yes | - |

#### `SOURCE/kbwin` — 4 function(s)

| function | PoL rva | Gold 2.1 va | slots moved | permutation | 1.0 witness | verdict |
|---|---|---|---:|---|---|---|
| `_WinMain@16` | 0x1bce0 | 0x484ae0 | 4 | reshuffle (uses unreferenced slots) | yes | - |
| `?AppAbout@@YGHPAXIIJ@Z` | 0x1c70e | 0x485530 | 2 | swap (2-cycle) | yes | - |
| `?SetMenuStatus@@YIXH@Z` | 0x1cce1 | 0x485b30 | 4 | 4-cycle | yes | **names** |
| `?SetMenus@@YIXPAXH@Z` | 0x1ceb8 | 0x485d10 | 4 | 4-cycle | yes | **names** |

#### `SOURCE/netwin` — 3 function(s)

| function | PoL rva | Gold 2.1 va | slots moved | permutation | 1.0 witness | verdict |
|---|---|---|---:|---|---|---|
| `@nb_init@8` | 0xa6c88 | 0x472b20 | 8 | reshuffle (uses unreferenced slots) | yes | - |
| `@nb_term@0` | 0xa6ecc | 0x472d70 | 6 | reshuffle (uses unreferenced slots) | yes | - |
| `?nb_thr_ctl@@YIXXZ` | 0xa7758 | 0x473630 | 5 | reshuffle (uses unreferenced slots) | yes | - |

#### `SOURCE/wingraph` — 7 function(s)

| function | PoL rva | Gold 2.1 va | slots moved | permutation | 1.0 witness | verdict |
|---|---|---|---:|---|---|---|
| `?DDAppPaint@@YIHPAX0@Z` | 0x35601 | 0x49b110 | 10 | reshuffle (uses unreferenced slots) | yes | - |
| `?DDInitializePalette@@YIXXZ` | 0x35b31 | 0x49b640 | 3 | 3-cycle | yes | - |
| `?DDCreateSurface@@YIPAUIDirectDrawSurface@@KKH@Z` | 0x35d1c | 0x49b830 | 8 | reshuffle (uses unreferenced slots) | yes | - |
| `?DDSetFullScreenStatus@@YIXH@Z` | 0x366b0 | 0x49c1e0 | 2 | swap (2-cycle) | yes | - |
| `?WGUpdatePalette@@YIXPAC@Z` | 0x36b8f | 0x49c6e0 | 3 | 3-cycle | yes | - |
| `?WGInitializePalette@@YIXXZ` | 0x36d76 | 0x49c8d0 | 2 | swap (2-cycle) | yes | - |
| `?SetGraphicsType@@YIHH@Z` | 0x37595 | 0x49d160 | 3 | 3-cycle | yes | **names** |

#### `SOURCE/x_arena` — 1 function(s)

| function | PoL rva | Gold 2.1 va | slots moved | permutation | 1.0 witness | verdict |
|---|---|---|---:|---|---|---|
| `?ArenaWindowHandler@@YIHAAUtag_message@@@Z` | 0xc03dc | 0x463aa0 | 2 | swap (2-cycle) | - | - |

#### `SOURCE/x_cdtest` — 2 function(s)

| function | PoL rva | Gold 2.1 va | slots moved | permutation | 1.0 witness | verdict |
|---|---|---|---:|---|---|---|
| `?CDTest_VerifyTrack@@YIHH@Z` | 0x4a061 | 0x4aa210 | 2 | swap (2-cycle) | - | - |
| `?CDTest_GenerateTable@@YIHPAD@Z` | 0x4a0f0 | 0x4aa2a0 | 6 | reshuffle (uses unreferenced slots) | - | - |

## 9. Consequences for this branch

- **Slot residuals are a name-search problem, not a codegen problem.** Do not grind
  spellings or operand orders on a function whose only defect is displacements; solve the
  bucket order with `homm2.core.od_slots` and verify with `homm2 audit od-oracle`.
- **Names inherited from the PoL line are fitted to the fork.** Where they disagree with
  Buka they are not "the dev names we got slightly wrong" - they are a different tree's
  identifiers. `homm2 od-frames` currently reports 334 functions with local-offset
  mismatches and 154 with reference-order mismatches out of 1700 audited.
- **Heroes II 1.0 is a second witness for the 2.1 layout.** Where Buka's body was changed
  by the 2003 rebuild and the frame is ambiguous, `img-sw/HEROES2W.EXE` carries the same
  identifiers as Gold 2.1 and can be read instead.
- **Gold 2.1 is the right diff target for this branch**, as `buka-is-gold-2-1` already
  says - now with a quantified reason: against 2.0 you inherit 258 wrong frames.

## 10. The re-fit campaign (2026-08-03)

Acting on §9: for functions that are already byte-correct apart from displacements, the
2.1 frame is recoverable by re-spelling locals. That is a deterministic solve, not a
search - `od_slots.solve_layout` is given the required stack order and returns names.

**Targeting.** `homm2 od-frames` mixes real code defects with slot defects, so the target
set is computed directly: disassemble our object and the delinked target, normalise the
object-relative branch rendering and trailing padding, and keep functions whose
instruction streams are identical *except* for `[ebp-disp]` fields. That yields **46**
functions - the ones a rename provably takes to 100%.

**Our side of the map** comes from the compiler, not from parsing source: the TU is
compiled with `/Z7` and the `S_BPREL32` records give every local's exact offset
(including inner-scope locals and compiler temps). `/Z7` does not change `/Od` codegen,
so those are the real slots. Proc records are keyed back to mangled symbols through the
`.debug$S` relocations.

**Naming rule.** A rename may only produce a spelling that means the same thing -
abbreviation (`destination`->`dest`), word-order swap (`gridX`->`xGrid`), a same-concept
synonym (`hex`/`square`/`cell`), or a dropped qualifier. Suffixes are only added when
they are true of the variable (`Index` when it is used as a subscript). **No Hungarian
prefixes**: measured across the tree, locals do not use them (`netwin.cpp`: 0 of 29), so
`nCount`/`bFlag` would be this decompilation's invention, not the devs'. When no faithful
spelling lands in the required bucket the function is left alone.

**Three frame rules this campaign had to learn (each was a wrong assumption first):**

1. **Scope groups.** The frame is `[function-scope locals in bucket order]`, then the
   `this` spill, then each inner block's locals in bucket order. So a local retail places
   *below* `this` must be declared in an inner block - `combatManager::LoadIcons` needed
   `heroColor` moved into the `if` that uses it, not renamed.
2. **`/Gr` parameter spills take part.** A fastcall function's register parameters are
   spilled into the negative frame and are ordered by the same identifier hash, so they
   can (and sometimes must) be renamed - and the rewrite has to reach the signature, not
   just the body. `CalcFileCRC` closed only once its `filename` parameter was renamed.
3. **Declaration hoisting is a free reordering lever.** Ties inside one bucket are broken
   by declaration order, but declarations are often inline-initialised mid-function.
   Splitting `T x = e;` into a hoisted `T x;` plus `x = e;` reorders the declarations
   while emitting exactly the same stores at `/Od`.

**Result:** 16 functions taken to exactly 100%, no functions lost; tree 1009 -> 1025
exact. The slot-only set went 46 -> 16.

| blocker | count | what it needs |
|---|---:|---|
| only unnamed compiler temps still move | 5 | a placement model for compiler temporaries |
| no faithful spelling reaches the required bucket | 4 | `type`, `filename`, `diffName`, and four one-letter scratch names in `TestDynamicWindow` are pinned to buckets no same-concept spelling reaches |
| structural one-offs | 3 | declaration order / scope cases needing hand work |
| partially fitted (99.9x) | 4 | one slot each still off after the group solve |

The remaining 26 are listed by `slotonly.json` in the session scratchpad; re-running the
scan after any build regenerates it.

## 11. Why the rest of the tree is not yet exact (residual taxonomy, 2026-08-03)

Closing functions needs a target list, so the whole non-exact population was classified
by comparing our object against the delinked target instruction-by-instruction, with
`[ebp-disp]` operands masked and each function cut to its *claimed* span (objdiff
compares that span, not the symbol-to-symbol range - measuring the wrong range was worth
25 phantom "exact" functions).

| class | count | what it is | fixable by source? |
|---|---:|---|---|
| body differs | ~380 | three or more diff blocks - genuinely incomplete reconstruction | yes, ordinary matching work |
| jump-table artefact | 113 | the delinked object embeds switch tables in `.text`; a linear decode reads them as instructions | no - already exact, only the scanner is fooled |
| non-instruction residual | 57 | our instruction stream is identical, yet objdiff is below 100 | no - infrastructure, see below |
| slot-only | 17 | identical except frame displacements | **yes - identifier re-fit** |
| one/two-instruction residual | 33 | a single constant, field offset or slot differs | yes, case by case |

The 57 with an identical instruction stream split into three infrastructure causes, none
of which a source edit can reach:

- **`/GX` EH scope-table addend** (~14 functions, the largest single class). The
  exception frame pushes a scope-table entry: ours `push 0`, retail `push 0xb`. That
  operand is a relocation *addend* - the per-function offset into the shared EH table -
  so it encodes the table's layout, not anything in the function.
- **`gConfig` sub-field spelling.** `movl 0x12e, %edx` (our `gConfig.combatSpeed`) versus
  `movl 0x0, %edx` (the delinker gave that exact address its own symbol). Same address,
  different spelling of the same reference - `canonicalize_relocs` is the place this is
  meant to be equalised.
- **Compiler-generated `$E` attribution.** A static-initialiser thunk is bundled into our
  function's symbol range but sits outside retail's, so the compared spans disagree
  (`armyGroup::DamageGroup`).

One dead end worth recording so it is not retried: `__real@8@...` float-constant COMDATs
are spelled `$anon_str_<sha>` in the delinked target, which looks like the same
content-identified family as `??_C@` pooled strings. Adding them to
`canonicalize_data_symbols._family` changed nothing measurable - those functions are held
by the `$E` and EH classes above, not by the float name.

## 12. Cross-version record of every function closed

`docs/cross-version-closed-functions.tsv` lists each function this campaign took to
100%, what its 2.1 frame required, and how PoL 2.0 relates to it:

| relation | count | meaning |
|---|---:|---|
| 2.0 differs | 14 | the two retail builds order this function's locals differently, so the 2.0-fitted identifiers provably could not reproduce 2.1 - several carry the 1.0 witness confirming 2.0 is the deviation |
| 2.0 identical | 2 | both retail builds share the frame; our spelling was simply never right for either |
| body changed | 2 | no 2.0 counterpart with an identical body, so the versions cannot be compared here |

That ratio is the campaign fact in miniature: **most functions that need a local re-fit on
this branch need it because they are anchored to the PoL 2.0 fork, not because the
reconstruction was careless.**

## Reproduction

Scripts used (session scratchpad, not committed): `census.py` (masked-body cross-version
matcher), `rulesearch.py` (alternative-rule search), `categorize.py` (this table),
`probe_slots.c` / `probe2.c` / `probe3.c` (compiler probes). The per-function data is in
`docs/cross-version-slot-categories.tsv`.
