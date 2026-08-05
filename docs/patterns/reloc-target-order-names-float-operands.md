# Resolve the relocation TARGET to see a float product's operand order

**Trigger.** A function is stuck a few points short, `homm2 sema disasm --diff`
reports the block clean, and a raw `llvm-objdump -d` comparison also looks
clean: same opcodes, same instruction lengths, same `-0xN(%ebp)`. Every
remaining row is an `flds`/`fmuls`/`fadds` against an **absolute** operand, and
both sides print the same masked text.

The rows are not identical. Both operands of the product are memory floats, so
each is a `disp32` covered by a `IMAGE_REL_I386_DIR32` relocation — and the
only difference is **which relocation sits on which opcode**. Symbol-masking
diffs cannot see it, because they mask exactly that field.

## The byte evidence

`philAI::ValueOfEventAtPosition`, `MAP_OBJECT_TREASURE_CHEST` arm (body 0x1fd):

```
retail (delinked)                          ours (before)
d9 05 <r:const_000ea7d8>  flds   0x0       d9 05 <r:gafAITurnCostResource+0x18>  flds  0x18
d8 0d <r:const_00130c08>  fmuls  0x0       d8 0d <r:__real@4@4009c800000000000000> fmuls 0x0
e8 <r:__ftol>             calll             e8 <r:__ftol>                          calll
```

Identical opcodes, identical lengths, identical operand *forms*. Resolve the
two targets to RVAs and the swap appears:

    const_000ea7d8   = 0xea7d8    the float literal 1600.0 in .rdata
    const_00130c08   = 0x130c08   = gafAITurnCostResource + 0x18 = [RES_GOLD]

Retail loads the **literal** first, so the literal is the source's LEFT
operand. Ours loads the array element first. Under `/Od`, `A * B` with two
memory floats is always `fld A; fmul B`, so the first `fld` names the left
operand — with no register pressure or scheduling involved.

```cpp
/* ours */   value = (i32)(gafAITurnCostResource[IDX(RES_GOLD)] * AI_TREASURE_CHEST_GOLD_AMOUNT);
/* retail */ value = (i32)(AI_TREASURE_CHEST_GOLD_AMOUNT * gafAITurnCostResource[IDX(RES_GOLD)]);
```

The same read closed fifteen sites in that one function, and the order is
**per site, not per function** — `MAP_OBJECT_OASIS`, `BUOY`, `TEMPLE`,
`FAERIE_RING`, `IDOL`, `FOUNTAIN` and `WATERING_HOLE` all keep
`AI_..._FACTOR * gpCurAIHero->m_aiFightValue`, while `GAZEBO`,
`TREE_OF_KNOWLEDGE` and the five training sites need
`gpCurAIHero->m_aiFightValue * AI_..._FACTOR`. Do not normalise a family.

## The same channel reads a sum of calls

The evaluation order of `f(a) + f(b) + f(c)` is source order under `/Od`
(measured against our own object), so the pushed immediates name the source's
addend order directly:

```
retail  push $0xd / push $0xc   -> ComputeUpgradeValue(CREATURE_ORC,   CREATURE_ORC_CHIEF)
        push $0x10 / push $0xf  -> ComputeUpgradeValue(CREATURE_OGRE,  CREATURE_OGRE_LORD)
        push $0x16 / push $0x15 -> ComputeUpgradeValue(CREATURE_DWARF, CREATURE_BATTLE_DWARF)
ours    push $0x16 / push $0x15 first  (DWARF, ORC, OGRE)
```

`MAP_OBJECT_HILL_FORT` and `MAP_OBJECT_FREEMANS_FOUNDRY` each needed their three
addends reordered; nothing else in either arm differed.

## How to look

`build/scratch-m4/sxs.py` (kept for reproduction) disassembles both objects over
the claimed span, aligns them by index, and prints each row with its relocation
target resolved through `build/gen/symbol_names.csv`:

```
  127 * 0x1fd  flds  0x0   {const_000ea7d8}  | 0x1fd  flds  0x18  {const_00130c08}
  128 * 0x203  fmuls 0x0   {const_00130c08}  | 0x203  fmuls 0x0   {__real@4@4009c800000000000000}
```

A row is a real divergence when both sides resolve and the RVAs differ. A row
where only the *symbol spelling* differs (`const_00133134` vs
`?giCurPlayerBit@@3EA`) is the ordinary delinker-naming noise described in
`reloc-rows-are-not-the-residual.md`. Our own float literals surface as
`__real@4@<80-bit hex>` / `__real@8@<80-bit hex>`, which decodes directly:
exponent `0x4001` = 2^2, mantissa `0xc0` = 1.5, so `__real@4@4001c000000000000000`
is `6.0f` — enough to confirm which named constant each site uses.
