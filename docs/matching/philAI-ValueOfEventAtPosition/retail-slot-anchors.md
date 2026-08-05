# `philAI::ValueOfEventAtPosition` (RVA 0x0008911d) — residual evidence

Two structural divergences were closed in this pass:

1. **MAP_OBJECT_SKELETON arm** — retail assigns through an `if`/`else`, not a
   `?:`. The ternary's join temp cost a frame slot, a store-and-reload at the
   join (`mov [tmp],edx; mov eax,[tmp]; mov [value],eax` where retail has
   `mov [value],edx`), and — because the extra temp exhausted the
   `eax/ecx/edx` rotation — a `push ebx`/`pop ebx` pair plus a one-register
   shift through the rest of the function (visible as B166's ours-27i vs
   retail-28i).
2. **Player-bit test** — retail reads the map byte as the LEFT operand of the
   `&` and keeps the pointer form:
   `!(*(mapExtra + x + MAP_WIDTH * y) & giCurPlayerBit)`. Ours had
   `!(giCurPlayerBit & mapExtra[MAP_WIDTH * y + x])`, which evaluates the
   global bit first and folds the row product before the base.

After both, `homm2 sema disasm 0x8911d --blocks --diff --lite` reports every
code block flow-exact and size-exact (only the two trailing jump-table data
"blocks" differ), the frame is `0xf8` on both sides, and the saved-register
set is `esi` on both.

## Remaining residual: named-local slot order

27 locals, all in one function-scope group, `this` at `-0xcc`. The /Od model
in `scripts/homm2/core/od_slots.py` reproduces OUR frame exactly (verified
name-for-name against `homm2 sema frames SOURCE/PHILAI ValueOfEventAtPosition`),
so this is a solve, not a search.

Anchors recovered from a 1:1 alignment of the two disassemblies
(`build/scratch-m4/slotmap.py`, retail slot -> our local):

| retail | local | retail | local |
|---|---|---|---|
| -0x4  | amount        | -0x64 | index |
| -0x14 | battleValue   | -0x8c | exitX |
| -0x18 | resource      | -0x90 | exitY |
| -0x28 | creatureFlag  | -0x94 | exitValue |
| -0x38 | purchaseValue | -0x98 | otherCell |
| -0x40 | cell          | -0x9c | bestExitY (low confidence) |
| -0x44 | value         | -0xa0 | bestExitX (low confidence) |
| -0x4c | creature      | -0xa4 | bestExitValue |
| -0x54 | purchaseCost  | -0xa8 | routeLiveChance (low confidence) |
| -0x5c | battleWon     | -0xac | currentValue |
|       |               | -0xc8 | resources |

The unanchored six (`cellState`, `eventState`, `creaturePurchaseState[9]`,
`resourceState[3]`, `rewardState[2]`, `combatState[4]`) fill the gaps; the
36-byte `creaturePurchaseState` is pinned to `-0x88` by the only 0x24-wide
gap (`[-0x88, -0x64)`), and the 12-byte `resourceState` to `-0x10` by the
`[-0x10, -0x4)` gap. The three low-confidence rows above (1-2 aligned hits
each) conflict with `routeLiveChance` being 16 bytes and must be re-derived
before a bucket solve is attempted.

The `_x` disambiguation suffixes a previous lane had introduced were removed
in this pass (97.91% -> 97.83%, readable source); the score difference is
noise between two equally-wrong permutations.
