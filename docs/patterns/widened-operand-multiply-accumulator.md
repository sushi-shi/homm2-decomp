# `imul reg, mem` (folded) vs `mov reg, mem; imul reg, reg`: which side of `*` the narrow operand is on

**Trigger.** A product of one `u16`/`i16` member and one 32-bit member. VC6 `/Od`
ALWAYS widens the narrow operand into a register first, whichever side it is on;
what the source order decides is which operand owns the **destination** register:

- narrow operand on the **left**  -> `xor r,r; mov r16, narrow; imul r, [wide]`  (wide folded)
- narrow operand on the **right** -> `xor r,r; mov r16, narrow; mov r2, [wide]; imul r2, r`

The destination register belongs to the source's LEFT operand, so a left operand
that lives in memory has to be materialised, which is the extra `mov`.

Measured on `combatManager::GetClosestArmy` (0x17d6e) and
`combatManager::GetModLichDamage` (0x1722b); `m_monster.hitPoints` is `u16`,
`m_quantity` is `i32`.

## The divergence

```
ours    hitPoints * m_quantity                    retail   m_quantity * hitPoints
--------------------------------------------      --------------------------------------------
8b 55 e8        movl -0x18(%ebp), %edx            8b 55 e8        movl -0x18(%ebp), %edx
33 c0           xorl %eax, %eax                   33 c0           xorl %eax, %eax
66 8b 82 c2..   movw 0xc2(%edx), %ax              66 8b 82 c2..   movw 0xc2(%edx), %ax
8b 55 e8        movl -0x18(%ebp), %edx            8b 55 e8        movl -0x18(%ebp), %edx
0f af 82 96..   imull 0x96(%edx), %eax            8b 92 96 00..   movl 0x96(%edx), %edx
                                                  0f af d0        imull %eax, %edx
2b c8           subl %eax, %ecx                   2b ca           subl %edx, %ecx
```

Both sides widen `m_quantity`... no: both widen the `u16` `hitPoints` at `0xc2`
first. Retail then LOADS the `i32` `m_quantity` at `0x96` into `edx` and
multiplies into `edx`, so `m_quantity` is the source's left operand.

## What made it match

```cpp
            val = val * COMBAT_AI_DISTANCE_WEIGHT
                  - target->m_quantity * target->m_monster.hitPoints;
```
```cpp
    float remainingHitPoints = static_cast<float>(
        target->m_quantity * target->m_monster.hitPoints - target->m_hitPointsLost
    );
```

`GetClosestArmy` 96.96% -> EXACT, `GetModLichDamage` 93.31% -> EXACT.

## Caveat

Do not read the emission ORDER as the source order here: the narrow operand is
emitted first in both spellings because it needs the `xor`+`movw` widening. Only
the `imul` destination names the left operand. This is the `imul` sibling of
[add-simple-operand-into-accumulator](add-simple-operand-into-accumulator.md).
