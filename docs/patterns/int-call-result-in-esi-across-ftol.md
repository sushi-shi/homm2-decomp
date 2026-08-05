# `push esi` + `mov esi,eax` around `__ftol`: the int call result is the LEFT operand of `+`, not part of the float expression

**Trigger.** A `/Od` body that otherwise never needs a callee-saved register opens
with `pushl %esi` after `subl $N, %esp`, and somewhere inside there is
`mov %eax,%esi` right after a call, `call __ftol`, `add %eax,%esi`. Our version has
no `push esi`, one extra frame slot, and a `fild` of that slot.

The two shapes come from the same arithmetic written two ways:

```c
deadline = KBTickCount() + (i32)(9.0f * gfCombatSpeedMod[gConfig.combatSpeed]);   /* retail */
deadline = (i32)(KBTickCount() + gfCombatSpeedMod[gConfig.combatSpeed] * 9.0f);   /* ours   */
```

Both are "tick count plus a speed-scaled delay", and both round the same way for
the values this game passes, so semantics do not separate them — only the bytes do.

## The divergence (`DoRipple`, RVA 0xcb6b0, `BASE/Ripple`)

```
retail  KBTickCount() + (i32)(9.0f * gfCombatSpeedMod[...])
  e8 ..              calll   @PollSound@0
  e8 ..              calll   ?KBTickCount@@YIJXZ
  8b f0              movl    %eax, %esi            <- int result parked in a callee-saved reg
  8b 15 ..           movl    gConfig.combatSpeed, %edx
  d9 05 ..           flds    <9.0f>                <- constant is the LEFT factor: loaded first
  d8 0c 95 ..        fmuls   ?gfCombatSpeedMod@@3PAMA(,%edx,4)
  e8 ..              calll   __ftol
  03 f0              addl    %eax, %esi            <- the integer add happens AFTER the cast
  89 b5 44 fd ff ff  movl    %esi, -0x2bc(%ebp)

ours    (i32)(KBTickCount() + gfCombatSpeedMod[...] * 9.0f)
  e8 ..              calll   @PollSound@0
  e8 ..              calll   ?KBTickCount@@YIJXZ
  89 85 18 fd ff ff  movl    %eax, -0x2e8(%ebp)    <- extra frame slot for the int
  db 85 18 fd ff ff  fildl   -0x2e8(%ebp)          <- int joins the FLOAT expression
  8b 15 2e 01 00 00  movl    gConfig.combatSpeed, %edx
  d9 04 95 ..        flds    ?gfCombatSpeedMod@@3PAMA(,%edx,4)
  d8 0d ..           fmuls   <9.0f>
  de c1              faddp   %st, %st(1)
  e8 ..              calll   __ftol
  89 85 44 fd ff ff  movl    %eax, -0x2bc(%ebp)
```

Three independent tells, any one of which is enough:

- **`push esi` in the prologue.** `A + B` where `B` contains a call keeps `A` in
  `esi` across it. No `esi` in the frame means the integer never crossed a call —
  i.e. it was folded into the float expression instead.
- **`fild` of a frame slot.** An `int` operand only reaches the x87 stack when the
  source put it *inside* the parenthesised float expression. That `fild` slot is a
  front-end expression temp, so it also shifts the frame size by 4.
- **`flds <const>` before `fmuls <array>`** names the multiply order: the operand
  loaded by `fld` is the source's LEFT factor. `9.0f * gfCombatSpeedMod[i]` and
  `gfCombatSpeedMod[i] * 9.0f` are not interchangeable.

## What made it match

```c
        deadline7 =
            KBTickCount() + static_cast<i32>(9.0f * gfCombatSpeedMod[gConfig.combatSpeed]);
```

`DoRipple` 74.75% -> EXACT (`BASE/Ripple` unit to 100%). The same read fixed
`DoBlur`'s tail (RVA 0xcba60): `flds <350.0f>; fmuls gfCombatSpeedMod(,%edx,4);
call __ftol; mov %eax,%ecx; call ?DelayMilli@@YIXJ@Z` is
`DelayMilli(static_cast<i32l>(350.0f * gfCombatSpeedMod[gConfig.combatSpeed]))`.

**Caveat: this family is spelled both ways in the tree.** `SOURCE/SPELLS` really
does use the `fild` form — e.g. `deadline7 = static_cast<i32>(KBTickCount() +
gfCombatSpeedMod[gConfig.combatSpeed] * frameDelay)` is exact there. Read the
prologue of the function in front of you; do not propagate either spelling by
family.
