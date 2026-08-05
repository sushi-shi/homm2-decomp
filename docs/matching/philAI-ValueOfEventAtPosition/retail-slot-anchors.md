# `philAI::ValueOfEventAtPosition` (RVA 0x0008911d, 0x19b2) — closed EXACT

The function is byte-identical to retail. This is the record of what the bytes
proved, kept because the frame table is the specification for the thirty locals
and the operand-order census is reusable evidence.

## What earlier passes had already closed

1. **MAP_OBJECT_SKELETON arm** — retail assigns through an `if`/`else`, not a
   `?:`. The ternary's join temp cost a frame slot, a store-and-reload at the
   join, and a `push ebx`/`pop ebx` pair.
2. **Player-bit test** — retail reads the map byte as the LEFT operand of the
   `&` and keeps the pointer form (`MAP_EXTRA_AT_WFIRST(x, y) & giCurPlayerBit`).

## What this pass found

Everything below was invisible to `homm2 sema disasm --diff` (which renders
`-0xN(%ebp)` as `<addr>`) *and* to a raw `llvm-objdump -d` comparison (the
opcodes and lengths agree). It only appears once each relocation is resolved to
an absolute RVA — see `reloc-target-order-names-float-operands.md`.

**Fifteen float products had their operands the wrong way round.** Retail's
per-site orders:

| arm | retail spelling |
| :-- | :-- |
| `TREASURE_CHEST` | `AI_TREASURE_CHEST_GOLD_AMOUNT * gafAITurnCostResource[GOLD]` |
| `CAMPFIRE` | `AI_CAMPFIRE_GOLD_AMOUNT * gafAITurnCostResource[GOLD]` FIRST, then the six-resource average; the sum runs WOOD, ORE, CRYSTAL, SULFUR, MERCURY, GEMS |
| `GAZEBO`, `TREE_OF_KNOWLEDGE` 1/2/3, `FORT`, `WITCH_DOCTOR_HUT`, `STANDING_STONES`, `MERCENARY_CAMP`, `XANADU` | `gpCurAIHero->m_aiFightValue * AI_..._FACTOR` |
| `TREE_OF_KNOWLEDGE` 2/3 costs | `AI_TREE_KNOWLEDGE_GOLD_COST * gafAITurnCostResource[GOLD]` (and the gem twin) |
| `MAGIC_GARDEN`, `FLOTSAM` x2, `SEA_CHEST` x3 | `<amount> * gafAITurnCostResource[...]` |
| `OASIS`, `BUOY`, `TEMPLE`, `FAERIE_RING`, `IDOL`, `FOUNTAIN`, `WATERING_HOLE` | already correct as `AI_..._FACTOR * m_aiFightValue` — the family is NOT uniform |

Six more structural facts:

- **`DAEMON_CAVE` grouping.** Retail is `A + (B + artifactValue) + (C + goldTerm)
  + penalty`, i.e. each daemon-cave outcome is its own parenthesised sum. Ours
  had one flat left-associated chain; the `faddp` positions name the grouping.
- **`ABANDONED_MINE`.** `gMineCharacteristics[GOLD] * gafAITurnCostResource[GOLD]
  * *(gaiTurnValueOfMine + x + y * MAP_WIDTH)` — the two per-resource factors
  multiply first (`fildl`/`fmuls`), the map byte last (`fimull`), and the index
  is the pointer form with the row product on the right.
- **Travel-gate scan.** `for (gateY = 0; gateY < MAP_HEIGHT; gateY++)`, not
  `MAP_HEIGHT > gateY`: retail loads the counter and `jge`s.
- **The two equality guards** read the scanned cell first:
  `exitCell->m_triggerType == theCell->m_triggerType`.
- **`HILL_FORT` / `FREEMANS_FOUNDRY` addend order.** The pushed creature ids
  give retail's order as ORC, OGRE, DWARF and IRON_GOLEM, PIKEMAN, SWORDSMAN.
- **Final adjacent-monster read** is `MAP_EXTRA_AT(x, y)` (base + column first),
  not the `MAP_WIDTH * y + x` subscript.

## The frame

Thirty function-scope locals, `this` at `-0xcc`, nothing between the deepest
local and the spill — so this function has **no** front-end expression or
inline-expansion slots and every one of the thirty is in the bucket sort.
`routeLiveChance` is a scalar in retail, not an array: `lea -0xa8(%ebp)` is
immediately followed by `bestExitValue` at `-0xa4`.

Nine of the thirty are declared-but-unused (`dead-local-frame-gap`): three
`i32[3]`, five `i32` and one `i32[9]`, filling 92 bytes of holes whose extents
are pinned by the used slots either side.

| retail | role | name | b | retail | role | name | b |
| :-- | :-- | :-- | --: | :-- | :-- | :-- | --: |
| -0x4 | resource amount | `amount` | 4 | -0x5c | battle-won flag | `bBattleWon` | 8 |
| -0x10 | dead[3] | `purchaseState` | 4 | -0x60 | dead | `pyramidState` | 8 |
| -0x14 | ChooseEvaluateBattle prize | `prize` | 6 | -0x64 | loop index | `i` | 9 |
| -0x18 | ResourceType | `res` | 6 | -0x88 | dead[9] | `townState` | 9 |
| -0x24 | dead[3] | `cellState` | 6 | -0x8c | scan column | `gateX` | 10 |
| -0x28 | free-join flag | `freeFlag` | 6 | -0x90 | scan row | `gateY` | 11 |
| -0x34 | dead[3] | `artifactState` | 6 | -0x94 | exit value | `exitRV` | 11 |
| -0x38 | replacement slot | `armySlot` | 6 | -0x98 | scanned cell | `exitCell` | 11 |
| -0x3c | dead | `oracleState` | 6 | -0x9c | best row | `chosenExitY` | 12 |
| -0x40 | event cell | `theCell` | 6 | -0xa0 | best column | `chosenExitX` | 13 |
| -0x44 | accumulator | `eventRV` | 8 | -0xa4 | best value | `bestRV` | 15 |
| -0x48 | dead | `combatState` | 8 | -0xa8 | live-chance out | `exitLiveChance` | 15 |
| -0x4c | creature to buy | `buyCreature` | 8 | -0xac | current position value | `positionValue` | 15 |
| -0x50 | dead | `lampState` | 8 | -0xc8 | RVConversion vector[7] | `costList` | 15 |
| -0x54 | purchase count | `numToBuy` | 8 | | | | |

Buckets are non-decreasing down that list and declaration order is the reverse
of the slot order **within each equal-bucket run**; across runs it is free, so
the source groups them by purpose. The three roles that gave up their obvious
name (`cell`, `value`, `creature`) could not keep it: `bucket(cell) = 14` and
`bucket(value) = 1`, but retail needs `bucket(cell) <= bucket(value)`, and no
spelling of `value` reaches 14 without stranding the fourteen roles below it.
The names chosen instead come from the callees' own vocabulary — `numToBuy` and
`armySlot` are `EvaluateOneTimeCreaturePurchase`'s purchase count and
replacement army slot, `prize` is `ChooseEvaluateBattle`'s prize argument, and
`eventRV`/`exitRV`/`bestRV` follow the file's own RV convention
(`gaiHeroEventStratRVOfPos`, `gArtifactBaseRV`, `RVConversion`).

## Reproduce

```sh
python3 build/scratch-m4/sxs.py SOURCE/PHILAI \
    '?ValueOfEventAtPosition@philAI@@QAEHHHHPAH@Z' 0x19b2   # rows + resolved relocs
python3 build/scratch-m4/layout_check.py                    # frame model vs retail
```
