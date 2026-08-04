# `a = K; b = K;` — not `a = K; b = a;`

**Trigger.** Two (or more) neighbouring fields end up holding the same constant and
retail stores the immediate into *each* of them, while ours stores it once and then
copies field to field. Every copied assignment costs one extra load, so the block is
`2 * (n - 1)` instructions too long and the register rotation downstream is shifted.

Measured on `game::SetupOrigData` (RVA 0x4df08), four sites: the calendar triple, the
hero patrol pair, the hero destination pair and the ultimate-artifact pair.

## The bytes (VC6 SP5, `/Od /MT /Gr /G5 /Ob1 /Gf /Gi- /GX`)

`m_month`, `m_week`, `m_day` are three adjacent `i16` fields at 0x49a / 0x498 / 0x496.

```
ours   m_month = 1; m_week = m_month; m_day = m_week;
--------------------------------------------------------
66 c7 80 9a 04 00 00 01 00  movw $0x1, 0x49a(%eax)
8b 4d f4                    movl -0xc(%ebp), %ecx
8b 55 f4                    movl -0xc(%ebp), %edx
66 8b 82 9a 04 00 00        movw 0x49a(%edx), %ax     <<< reload m_month
66 89 81 98 04 00 00        movw %ax, 0x498(%ecx)
8b 4d f4                    movl -0xc(%ebp), %ecx
8b 55 f4                    movl -0xc(%ebp), %edx
66 8b 82 98 04 00 00        movw 0x498(%edx), %ax     <<< reload m_week
66 89 81 96 04 00 00        movw %ax, 0x496(%ecx)

retail  m_month = 1; m_week = 1; m_day = 1;
--------------------------------------------------------
66 c7 80 9a 04 00 00 01 00  movw $0x1, 0x49a(%eax)
8b 4d f4                    movl -0xc(%ebp), %ecx
66 c7 81 98 04 00 00 01 00  movw $0x1, 0x498(%ecx)
8b 55 f4                    movl -0xc(%ebp), %edx
66 c7 82 96 04 00 00 01 00  movw $0x1, 0x496(%edx)
```

The same shape appears for a chained `-1`:

```
ours   rec.m_patrolY = -1; rec.m_patrolX = rec.m_patrolY;   (8 instructions, uses %esi)
retail rec.m_patrolY = -1; rec.m_patrolX = -1;              (4 instructions)
```

## What made it match

Spell every member of the group with its own literal:

```cpp
    m_month = INITIAL_CALENDAR_VALUE;
    m_week = INITIAL_CALENDAR_VALUE;
    m_day = INITIAL_CALENDAR_VALUE;
...
        m_heroRecs[i].m_patrolY = HERO_DESTINATION_NONE;
        m_heroRecs[i].m_patrolX = HERO_DESTINATION_NONE;
```

`game::SetupOrigData` 94.36% -> EXACT (the four sites were its entire residual; the
apparent register-allocation differences in the seven downstream blocks were only the
rotation phase shifted by the extra temporaries).
