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

## Sibling inner scopes, and why `switch` dispatchers hoist

Two more facts, measured on `widget::Main` (0xd4180) and `iconWidget::Main`
(0xbba10) - both message dispatchers with one `switch` and three locals.

1. **Sibling inner scopes are laid out in source order, shallowest first.** With
   `x`/`y` declared in the mouse `case` block and `flags`/`frame` declared in an
   earlier `case`'s `if` block, VC6 gave the *earlier* case's local the shallowest
   slot regardless of bucket:

   ```
   ours (x,y and frame each in their own case block)   retail
   -0x04  frame     <- earlier case, deeper nesting    -0x04  x
   -0x08  x                                            -0x08  y
   -0x0c  y                                            -0x0c  frame
   -0x10  this                                         -0x10  this
   ```

   The whole residual was invisible in the score: `iconWidget::Main` read 99.97%
   and `widget::Main` 99.96% with every hit-test local on the wrong slot. Only a
   raw byte diff with the `(%ebp)` displacements kept (`0xfc` vs `0xf4`, `0xf8` vs
   `0xfc`, ...) shows it.

2. **A `switch` arm may not jump past an initialisation** (`[stmt.dcl]`), so any
   local that a `case` initialises has to be declared before the `switch` unless
   the arm gets its own braces. That is why retail's dispatchers carry
   `i16 x; i16 y;` at function scope and assign inside the arm, while a local born
   inside an `if` body (`frame`, `flags`) legally stays nested - and lands in the
   deep group. Expect the split, not a uniform hoist:

   ```cpp
   MessageDispatchResult iconWidget::Main(tag_message& msg) {
       i16 x;                                        /* function scope */
       i16 y;
       switch (msg.type) {
           case MESSAGE_WIDGET:
               switch (msg.payload.widget.command) {
                   case WIDGET_COMMAND_SET_FRAME:
                       if (msg.payload.widget.id == m_id) {
                           i16 frame = ...;          /* stays nested */
   ...
           case MESSAGE_LEFT_BUTTON_DOWN:
           case MESSAGE_RIGHT_BUTTON_DOWN:
               x = msg.payload.mouse.x - m_owner->m_posX;
   ```

   Both functions went EXACT on the first rebuild after the split. The buckets
   were already right (`x`=8, `y`=9); only the scope grouping was wrong.

   The counter-example is in the same batch: `border::Main` and `button::Main` have
   *only* `x`/`y`, so there is no second group to order and both matched with the
   pair declared inside the `case` block.
