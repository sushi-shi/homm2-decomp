# `army::SpecialAttack` (RVA 0x0001a1d9, 0x165f) — closed EXACT

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

## How it closed

Our names were the obvious English role names; their bucket sequence has 23
drops and its longest non-decreasing subsequence is 15 of 50, so at least 35 had
to be respelled.  A curated ~12-per-role vocabulary is INFEASIBLE (dies at
`effectType`).  What worked is in
`docs/patterns/pair-linked-bucket-chain-solve.md`: a compositional vocabulary,
pair-LINKED stems (sixteen of the fifty are coordinate pairs whose two members
sit up to 46 positions apart), and a beam with **per-floor quotas** so a cheap
prefix cannot strand a wide-spread pair commitment.  Feasibility was settled
first, with no cost function at all: filtered to `pen <= 2` candidates the walk
reaches position 50, so a readable solution provably exists.

The landed set keeps eleven roles' natural names and needs no stretched
spelling.  Notable recoveries: `targetX`/`targetY` are `xCentre`/`yCentre`
(they come from `target->MidX()`/`MidY()`), `sourceX`/`sourceY` are
`anchorX`/`anchorY`, `missileHalfWidth`/`Height` are `arrowHalfW`/`arrowHalfH`,
`minX`/`minY` are `clipLeft`/`clipTop`, and the loop counter reused by three
loops is `k`.

Declaration order is the reverse of the slot order **within each equal-bucket
run**; across runs it is free, so the source lists the runs in ascending bucket
order, which keeps `castX`/`castY`, `gainX`/`gainY` and `inFlightX`/`inFlightY`
adjacent.

## The frame was masking two source facts

With all 50 slots finally identical, a raw byte compare (masking only bytes a
relocation covers on either side) left three differing bytes, both real:

```
0x0961  target  jmp <body 0xefa>          |  ours  jmp <body 0xf55>
0x10b4  target  movl -0x3f4(%ebp), %eax   |  ours  movl -0x3a0(%ebp), %eax
0x10ba  target  cmpl -0x3a0(%ebp), %eax   |  ours  cmpl -0x3f4(%ebp), %eax
```

- the mage/bolt arm's `jmp` lands **inside** the ammo-cart block (body 0xefa),
  not past it: `m_monster.shots--` sits AFTER the mage/missile `if`/`else` and
  is common to both arms, so a mage also spends a shot;
- the lich splash identity test loads the victim first:
  `splashTarget != pEnemy`, not `pEnemy != splashTarget`.

Both were invisible while the permutation was live.  Order matters: **solve the
frame first, then re-diff.**

## Reproduce

```sh
homm2 sema frames SOURCE/ARMY SpecialAttack
python3 build/scratch-m4/sm4.py  SOURCE/ARMY '?SpecialAttack@army@@QAEXXZ' 0x165f
python3 build/scratch-m4/sa_run.py 4 60000 0.0 1
python3 build/scratch-m4/sa_apply.py build/scratch-m4/sa_map5.txt
```
