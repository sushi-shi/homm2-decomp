# `army::SpecialAttack` (RVA 0x0001a1d9) — retail frame evidence

The instruction stream is byte-identical to ours modulo `-0xN(%ebp)`
displacements: `homm2 sema disasm 0x1a1d9 --blocks --diff --lite` reports
185/186 blocks exact, and a 1:1 alignment of the two disassemblies yields a
clean 74-slot bijection with no ambiguity. What remains is purely the /Od
named-local slot ORDER.

## Frame anatomy

Both sides: `push ebp; mov ebp,esp; push -1; push <unwind+K>; mov eax,fs:[0];
push eax; mov fs:[0],esp; sub esp,0x43c; push esi`. The three slots at `-0x4`,
`-0x8`, `-0xc` are the /GX `EXCEPTION_REGISTRATION` record (they map to
themselves), so the 52 named locals occupy `-0x10 .. -0x448`, `this` is at
`-0x418`, and everything below is compiler temps.

`combatText` is 800 bytes (`char[ARMY_COMBAT_TEXT_SIZE]`); the other 51 locals
are 4 bytes each.

## Retail slot order (shallow -> deep), keyed by our role names

| # | retail slot | role | # | retail slot | role |
|---|---|---|---|---|---|
| 1 | -0x10 | oldX | 27 | -0x394 | backgroundX |
| 2 | -0x14 | directionFrame | 28 | -0x398 | distance |
| 3 | -0x18 | targetX | 29 | -0x39c | slope |
| 4 | -0x1c | sourceY | 30 | -0x3a0 | target |
| 5 | -0x33c | combatText (800 B) | 31 | -0x3a4 | originalFacing |
| 6 | -0x340 | effectX | 32 | -0x3a8 | backgroundY |
| 7 | -0x344 | oldY | 33 | -0x3ac | missileHalfWidth |
| 8 | -0x348 | missileSteps | 34 | -0x3b0 | missileBackground |
| 9 | -0x34c | reverseMissile | 35 | -0x3b4 | minX |
| 10 | -0x350 | sourceColumn | 36 | -0x3b8 | direction |
| 11 | -0x354 | effectY | 37 | -0x3bc | minY |
| 12 | -0x358 | effectType | 38 | -0x3c0 | missileDelay |
| 13 | -0x35c | yDistance | 39 | -0x3c4 | killed |
| 14 | -0x360 | sourceRow | 40 | -0x3c8 | maxX |
| 15 | -0x364 | targetColumn | 41 | -0x3cc | missileX |
| 16 | -0x368 | currentMissileX | 42 | -0x3d0 | missileHalfHeight |
| 17 | -0x36c | xStep | 43 | -0x3d4 | initialXDistance |
| 18 | -0x370 | xDistance | 44 | -0x3d8 | damage |
| 19 | -0x374 | endX | 45 | -0x3dc | maxY |
| 20 | -0x378 | attackDirection | 46 | -0x3e0 | missileY |
| 21 | -0x37c | targetY | 47 | -0x3e4 | initialYDistance |
| 22 | -0x380 | currentMissileY | 48 | -0x3e8 | originalAttack |
| 23 | -0x384 | yStep | 49 | -0x3ec | angle |
| 24 | -0x388 | endY | 50 | -0x3f0 | sourceX |
| 25 | -0x38c | missileSpacing | 51 | -0x3f4 | splashTarget |
| 26 | -0x390 | targetRow | 52 | -0x3f8 | adjacentHex |

None of the 52 carries an initializer (the prologue's only stores are the
`this` spill and the two plain statements `damage = 0; killed = 0;`), so
declaration order is a free tie-break: the only constraint is that
`bucket(name)` be NON-DECREASING down this list.

## Why it is left open

A DP over that monotone chain was run against three vocabularies:

1. the natural role names above — 23 bucket drops, infeasible;
2. a curated per-role list where every candidate is a spelling a 1996 dev
   could plausibly have written (~20 per role, both descriptive and short
   C-style forms) — INFEASIBLE, the chain dies at slot 33 with only bucket 15
   reachable;
3. a widened combinatorial vocabulary (synonym cross-products plus 2-4 letter
   abbreviations) — FEASIBLE, but the cheapest solution renames roughly thirty
   of the fifty-two locals to stilted forms (`columnSelf`, `rowSource`,
   `hitFacing`, `srcPosX`, `lowPosX`, `adjCell`, `way` for a distance, `hurt`
   for damage, ...).

Under the project's rule that a clean 99.9 beats a contorted 100, arm 3 was
not landed. The `_NN` suffixes a previous lane had introduced WERE removed
(they are the same contortion in a more obvious form) — that alone moved the
function from 99.76% to 99.78% and restored readable source.

If the real 2.1 local names ever surface, the table above is the whole
specification: assign them in this order, check `bucket()` is non-decreasing,
and pick any declaration order that puts later-slotted names earlier within
each equal-bucket run.

## Reproduce

```sh
homm2 sema disasm 0x1a1d9 --blocks --diff --lite     # 185/186 blocks exact
homm2 sema frames SOURCE/ARMY SpecialAttack          # our side's 52 names
python3 scripts/homm2/core/od_slots.py order <names> # model, validated here
```
