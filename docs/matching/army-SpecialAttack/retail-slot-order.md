# `army::SpecialAttack` (RVA 0x0001a1d9, 0x165f) — retail frame evidence

The instruction stream is byte-identical to ours modulo `-0xN(%ebp)`
displacements. A 1:1 alignment of the two delinked/compiled objects over the
claimed span yields a clean 74-slot bijection with no ambiguity, and the 57
rows that a resolved-relocation diff still flags are all delinker naming noise
(`$alt_CIatan` for `_atan`, `$alt_CIsqrt` for `_sqrt`, `const_00124038` for
`gTargetName`, the `/GX` scope-table pointer, `$SG`/`$anon_str` string cells and
the intra-object recursive `call`). What remains is purely the `/Od` named-local
slot ORDER.

## Frame anatomy, in the five-region model

Both sides: `push ebp; mov ebp,esp; push -1; push <unwind+K>; mov eax,fs:[0];
push eax; mov fs:[0],esp; sub esp,0x43c; push esi`. `/GX`, so the three dwords
at `-0x4 .. -0xc` are the `EXCEPTION_REGISTRATION` record and the first local
starts at `-0x10`.

| band | ours | retail |
| :-- | :-- | :-- |
| function-scope named locals | `-0x10 .. -0x3f0` (50) | `-0x10 .. -0x3f0` (50) |
| inner-block named locals | `-0x3f4`, `-0x3f8` (2) | `-0x3f4`, `-0x3f8` (2) |
| front-end expression/expansion slots | `-0x3fc .. -0x414` (7) | `-0x3fc .. -0x414` (7) |
| `this` spill | `-0x418` | `-0x418` |
| back-end temps | `-0x41c .. -0x448` | `-0x41c .. -0x448` |

Every slot from `-0x3fc` down maps to **itself** in the bijection, and the rows
that write them align index-for-index. So the
`inline-expansion-slots-after-expression-temps` lever does **not** apply here:
no value in this function is misclassified between a named local and a
front-end slot, and neither side has a front-end slot the other lacks.

The inner-block group is exactly `{splashTarget, adjacentHex}`, declared inside
the `CREATURE_LICH` arm, on both sides. It is also **bucket-sorted**:
`bucket(splashTarget) = 7`, `bucket(adjacentHex) = 8`, and `splashTarget` is the
shallower of the two even though `adjacentHex` is declared first. So a second
group exists but it is already the right size — there is no third chain to
split the problem into.

The `/Od` model reproduces **our** frame exactly: 50/50 offsets, taking the
`/GX` base as `-0xc` and `sizeof(combatText) = 800`
(`build/scratch-m4/solve_sa5.py` control run).

## Retail slot order (shallow -> deep), keyed by our role names

`combatText` is 800 bytes (`char[ARMY_COMBAT_TEXT_SIZE]`); the rest are 4 bytes.

| # | retail slot | role | # | retail slot | role |
|---|---|---|---|---|---|
| 1 | -0x10 | oldX | 26 | -0x390 | targetRow |
| 2 | -0x14 | directionFrame | 27 | -0x394 | backgroundX |
| 3 | -0x18 | targetX | 28 | -0x398 | distance |
| 4 | -0x1c | sourceY | 29 | -0x39c | slope |
| 5 | -0x33c | combatText (800 B) | 30 | -0x3a0 | target |
| 6 | -0x340 | effectX | 31 | -0x3a4 | originalFacing |
| 7 | -0x344 | oldY | 32 | -0x3a8 | backgroundY |
| 8 | -0x348 | missileSteps | 33 | -0x3ac | missileHalfWidth |
| 9 | -0x34c | reverseMissile | 34 | -0x3b0 | missileBackground |
| 10 | -0x350 | sourceColumn | 35 | -0x3b4 | minX |
| 11 | -0x354 | effectY | 36 | -0x3b8 | direction |
| 12 | -0x358 | effectType | 37 | -0x3bc | minY |
| 13 | -0x35c | yDistance | 38 | -0x3c0 | missileDelay |
| 14 | -0x360 | sourceRow | 39 | -0x3c4 | killed |
| 15 | -0x364 | targetColumn | 40 | -0x3c8 | maxX |
| 16 | -0x368 | currentMissileX | 41 | -0x3cc | missileX |
| 17 | -0x36c | xStep | 42 | -0x3d0 | missileHalfHeight |
| 18 | -0x370 | xDistance | 43 | -0x3d4 | initialXDistance |
| 19 | -0x374 | endX | 44 | -0x3d8 | damage |
| 20 | -0x378 | attackDirection | 45 | -0x3dc | maxY |
| 21 | -0x37c | targetY | 46 | -0x3e0 | missileY |
| 22 | -0x380 | currentMissileY | 47 | -0x3e4 | initialYDistance |
| 23 | -0x384 | yStep | 48 | -0x3e8 | originalAttack |
| 24 | -0x388 | endY | 49 | -0x3ec | angle |
| 25 | -0x38c | missileSpacing | 50 | -0x3f0 | sourceX |
| | | | 51/52 | -0x3f4/-0x3f8 | splashTarget / adjacentHex (inner block, already exact) |

None of the 50 carries an initializer (the prologue's only stores are the `this`
spill and `damage = 0; killed = 0;`), so declaration order is a free tie-break:
the only constraint is that `bucket(name)` be NON-DECREASING down this list.

## Why it is left open — now with a measured bound

Our current names are the obvious English role names. Their bucket sequence has
**23 drops**, and its **longest non-decreasing subsequence is 15 of 50**. That
is a hard bound, not a search result: *any* layout-correct naming must respell
at least 35 of the 50 locals.

A generated vocabulary (stem synonyms crossed with the codebase's own
`i`/`n`/`b`/`p`/`f` prefixes and `Pos`/`Coord`/`Index`/`Num` decorations,
~40 candidates per role) puts a hard floor on how idiomatic the result can be:

| naturalness budget | outcome |
| :-- | :-- |
| pen 0-1 only (name or its first-choice synonym) | **INFEASIBLE**, dies at role 7 (`oldY`) |
| pen 0-2 (adds abbreviations, prefixes, `Pos` suffixes) | **INFEASIBLE**, dies at role 19 (`endX`) |
| pen 0-3 (adds stretches) | feasible; optimum keeps **8** natural names, needs 13 pen-1, 23 pen-2 and 6 pen-3 |

The cheapest feasible arm reads `iTargetPosX`, `sourceYPos`, `myPosColumn`,
`iTotalSteps`, `bMirror`, `pow` (for `effectType`), `iXDist`, `endPosX`,
`fTangent`, `nMissileHalfWidth`, `pSaveBitmap`, `clipLeft`/`clipTop` beside
`maxX`/`maxY`, `launchX` beside `iMissileY`, `fullYDistance`, `iOriginalAttack`,
`fAngle` and `iMyX` — and breaks essentially every X/Y pair in a function whose
whole subject is paired coordinates.

Under the project's rule that a clean 99.9 beats a contorted 100, that arm is
not landed. The `_NN` suffixes a previous lane had introduced WERE removed (they
are the same contortion in a more obvious form); that alone moved the function
from 99.76% to 99.78% and restored readable source.

If the real 2.1 local names ever surface, the table above is the whole
specification: assign them in this order, check `bucket()` is non-decreasing,
and pick any declaration order that puts later-slotted names earlier within each
equal-bucket run.

## Reproduce

```sh
homm2 sema frames SOURCE/ARMY SpecialAttack                  # our 52 names
python3 build/scratch-m4/sm4.py  SOURCE/ARMY '?SpecialAttack@army@@QAEXXZ' 0x165f
python3 build/scratch-m4/sxs.py  SOURCE/ARMY '?SpecialAttack@army@@QAEXXZ' 0x165f
python3 build/scratch-m4/solve_sa6.py 2                      # vocabulary DP
```
