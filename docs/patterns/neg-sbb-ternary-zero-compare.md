# neg-sbb-ternary-zero-compare

**Symptom.** A branchless two-constant select whose mask comes from
`neg`/`sbb` instead of `setcc`/`dec`:

    mov  ecx, <x>
    [sub ecx, K]          <-- present only when the source compares against K != 0
    neg  ecx
    sbb  ecx, ecx
    [and ecx, M]          <-- omitted when M == -1
    add  ecx, A

Ours emitted a two-arm `if`/`else` (two `mov mem, imm32` stores and a `jmp`)
in the same place, so the block is ~10 bytes longer and every following
displacement shifts.

Related but distinct: [od-constant-ternary-lowering](od-constant-ternary-lowering.md)
covers the `setcc` / `dec` / `and` / `add` shape VC6 uses when the condition is
a relational between two *variables*. This entry is the shape it uses when the
condition is an equality against a **compile-time constant** — VC6 then folds
the comparison into `sub` + `neg` + `sbb` and there is no `setcc` to read.

## The bytes (VC6 SP5, `/Od /MT /Gr /G5 /Ob1 /Gf /Gi- /GX`)

`army::GetAdjacentCellIndex` (RVA 0x7d539), the `COMBAT_DIRECTION_WIDE_EAST` arm:

```
ours   if (m_facing == ARMY_FACING_RIGHT)          retail  direction = m_facing == ARMY_FACING_RIGHT
           direction = COMBAT_DIRECTION_SOUTHWEST;                      ? COMBAT_DIRECTION_SOUTHWEST
       else                                                             : COMBAT_DIRECTION_SOUTHEAST;
           direction = COMBAT_DIRECTION_SOUTHEAST;
------------------------------------------------  ------------------------------------------------
8b 4d fc        mov  ecx, [ebp-4]                  8b 55 fc        mov  edx, [ebp-4]
83 b9 86 00 00 00 01  cmp [ecx+0x86], 1            8b 82 86 00 00 00  mov eax, [edx+0x86]
75 09           jne  <else>                        83 e8 01        sub  eax, 1
c7 45 0c 03 00 00 00  mov [ebp+0xc], 3             f7 d8           neg  eax
eb 07           jmp  <join>                        1b c0           sbb  eax, eax
c7 45 0c 02 00 00 00  mov [ebp+0xc], 2             83 c0 03        add  eax, 3
                                                   89 45 0c        mov  [ebp+0xc], eax
```

## Reading the arms back out

`sbb reg, reg` after `neg` yields **0 when the compared value was zero** and
`-1` otherwise, i.e. mask == 0 exactly when the source condition is TRUE.  The
tail is `(mask & (elseValue - thenValue)) + thenValue`:

    add K            -> thenValue = K
    and M            -> elseValue = M + K      (M omitted means M == -1, elseValue = K-1)
    sub C before neg -> condition is `x == C`  (absent means `x == 0`)

Worked on the two arms above:

    sub 1 / and -5 / add 5  ->  m_facing == 1 ? 5 : 0   (NORTHWEST : NORTHEAST)
    sub 1 /        / add 3  ->  m_facing == 1 ? 3 : 2   (SOUTHWEST : SOUTHEAST)

`army::ValidAttack` (0x7d361) has the same two selects with **no** leading
`sub`, which pins the condition to a compare against 0 — the source there is
`m_facing == ARMY_FACING_LEFT ? ... : ...` (ARMY_FACING_LEFT == 0), with the
arms swapped relative to GetAdjacentCellIndex. Our reconstruction had spelled
it `static_cast<u32>(m_facing) < static_cast<u32>(ARMY_FACING_RIGHT) ? ... : ...`,
which is semantically the same but emits `and ecx, 5` (mask polarity flipped)
and one instruction fewer.

`army::GetAttackMask` (0x7d080) uses the same shape twice over the WIDE
attribute test, including a byte-width mask when the two constants differ only
in the low byte:

```
retail  blockedMaskValue = HAS(m_monster.attributes, MONSTER_ATTRIBUTE_WIDE)
                               ? 0 : SPECIAL_DIRECTION_MASK;      /* 0 : 0xc0 */
8b 88 d0 00 00 00  mov ecx, [eax+0xd0]
83 e1 01           and ecx, 1
f7 d9              neg ecx
1b c9              sbb ecx, ecx
80 e1 40           and cl, 0x40        <-- byte form of `and ecx, 0xffffff40`
81 c1 c0 00 00 00  add ecx, 0xc0
```

**Close.** `army::GetAdjacentCellIndex` 59.32% -> EXACT, `army::GetAttackMask`
87.52% -> EXACT, `army::ValidAttack` 96.39% -> EXACT (SOURCE/PATH).
