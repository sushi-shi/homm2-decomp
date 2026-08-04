# A frame that interleaves "inner-block" locals: retail declared them at function scope

**Trigger.** Our source declares some locals inside a nested `{}` block. Under `/Od`
those get their own scope table and are laid out *after* the whole outer group, so
they land at the deepest slots. Retail's frame instead shows them **interleaved with
the outer locals**. That interleaving is only reachable from a single scope: the
retail source declared everything at function scope (C89 style), even the values that
are only used deep inside a loop.

Measured on `combatManager::SetCombatDirections` (RVA 0x2bc31) and
`combatManager::GetCommand` (RVA 0x2d56e).

## The evidence

`SetCombatDirections` declares `outputDirection`/`mappedDirection` inside the
direction-map `for` body and `previous`/`next` inside an `if` body. Ours therefore
puts them below `this`-adjacent outer slots; retail has them in the middle:

```
ours (2 nested scopes)                         retail (one scope)
-0x04  unresolved                              -0x08  standable[8]
-0x0c  pathValid[8]                            -0x0c  mappedDirection      <-- "inner"
-0x10  targetIndex                             -0x2c  directionHexes[8]
-0x14  currentArmy                             -0x30  targetArmy
-0x34  rearHexes[8]                            -0x34  outputDirection      <-- "inner"
-0x54  directionHexes[8]                       -0x38  targetIndex
-0x58  direction                               -0x3c  unresolved
-0x5c  targetArmy                              -0x44  pathValid[8]
-0x64  standable[8]                            -0x48  direction
-0x68  targetSide                              -0x4c  previous             <-- "inner"
-0x6c  mappedDirection   <-- inner block       -0x50  currentArmy
-0x70  outputDirection   <-- inner block       -0x54  next                 <-- "inner"
-0x74  next              <-- inner block       -0x74  rearHexes[8]
-0x78  previous          <-- inner block       -0x78  targetSide
-0x7c  this                                    -0x7c  this
```

Retail's 14 offsets are exactly `predict_offsets()` over **one** 14-name bucket sort
(verified by `homm2/core/od_slots.py`); no two-scope layout can produce them, because
a nested scope's locals are always contiguous at the bottom.

## What made it match

Hoist the declarations to the function's top (keeping the initialising statement where
it was, so statement order and codegen are unchanged), then re-solve the whole frame
as one bucket sort:

```cpp
    char standable_0[COMBAT_DIRECTION_COUNT];
    i32 directionHexes[COMBAT_DIRECTION_COUNT];
    i32 rearHexes_2[COMBAT_DIRECTION_COUNT];
    char pathValid_28[COMBAT_DIRECTION_COUNT];
    i32 outputDirection_7;      /* was declared in the for body */
    i32 mappedDirection_5;      /* was declared in the for body */
    i32 previous_1;             /* was declared in the if body  */
    i32 next_28;                /* was declared in the if body  */
    ...
        outputDirection_7 = direction_28;   /* assignment stays in place */
```

`SetCombatDirections` 97.60% -> EXACT, `GetCommand` 98.28% -> EXACT.

**Corollary.** The inverse also holds: if retail's frame keeps a group strictly below
every outer local (as `ProcessCombatMsg`'s `helpIndex` and `currentMouseX/Y` do), that
group really was declared in a nested block and must stay there.
