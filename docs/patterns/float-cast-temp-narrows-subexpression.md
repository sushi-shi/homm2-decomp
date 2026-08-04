# A lone `fst dword ptr [temp]` mid-expression: an inner `static_cast<float>`

**Trigger.** Retail's frame is one 4-byte slot larger than ours for every
occurrence, and somewhere in the middle of an x87 chain it stores st(0) with a
NON-popping `fst m32` (`d9 55 xx` / `d9 5d xx` without the `/3` pop form) to a
slot nothing ever reads back. The chain then continues on the same st(0).

That store is VC6's materialisation of a `float` conversion applied to a
`double`-valued SUBEXPRESSION. It is not the assignment narrowing: the final
store to the destination is a separate `fstp`. Wrapping the WHOLE expression in
`static_cast<float>` produces no such temp, because the assignment already
narrows; only a cast that sits *inside* the expression tree does.

## The divergence (`philAI::GetGameAttentionValue`, RVA 0x842f1)

```
ours   attention->gameWeightA =                     retail
         (float)(Random(0,100)/DIV + BASE);
------------------------------------------------  ------------------------------------------------
83 ec 18  sub  esp, 0x18                           83 ec 20  sub  esp, 0x20     <- 2 slots bigger
...                                                ...
89 45 f4  mov  [ebp-0xc], eax   ; Random result    89 45 f4  mov  [ebp-0xc], eax
db 45 f4  fild dword [ebp-0xc]                     db 45 f4  fild dword [ebp-0xc]
dc 35 ..  fdiv qword [DIVISOR]                     dc 35 ..  fdiv qword [DIVISOR]
                                                   d9 55 f0  fst  dword [ebp-0x10]   <- the cast
dc 05 ..  fadd qword [BASE]                        dc 05 ..  fadd qword [BASE]
8b 45 fc  mov  eax, [ebp-4]                        8b 45 fc  mov  eax, [ebp-4]
d9 18     fstp dword [eax]                         d9 18     fstp dword [eax]
```

Two of these in the same function, so retail's frame is 8 bytes larger and its
two int temps and two float temps interleave (`-0xc` int, `-0x10` float,
`-0x14` int, `-0x18` float) exactly as the declaration order predicts.

## What made it match

```cpp
    attention->gameWeightA = static_cast<float>(Random(0, 100) / AI_ATTENTION_RANDOM_DIVISOR)
                             + AI_ATTENTION_RANDOM_BASE;
```

The cast moved from around the whole sum onto the division alone.
`philAI::GetGameAttentionValue` 85.55% -> EXACT.

## Second confirmation (`philAI::RedistributeTroops`, RVA 0x87687)

Same shape in front of `__ftol`, i.e. a `float` cast nested inside an `i32` one:

```
ours                                               retail
------------------------------------------------  ------------------------------------------------
da b2 02 ..  fidiv dword [edx+fightValue]          da b2 02 ..  fidiv dword [edx+fightValue]
dc 05 ..     fadd  qword [ROUNDING]                dc 05 ..     fadd  qword [ROUNDING]
                                                   d9 55 d4     fst   dword [ebp-0x2c]
e8 ..        call  __ftol                          e8 ..        call  __ftol
89 45 dc     mov   [ebp-0x24], eax                 89 45 dc     mov   [ebp-0x24], eax
```

```cpp
            howMany = static_cast<i32>(static_cast<float>(
                static_cast<double>(transferBudget)
                    / gMonsterDatabase[IDX(sourceArmy->m_creatureTypes[bestSlot])].fightValue
                + AI_TROOP_REDISTRIBUTION_ROUNDING
            ));
```

`philAI::RedistributeTroops` 89.88% -> EXACT (with the other axes in the same
function).

Related: [fidiv-vs-fild-fdivp](fidiv-vs-fild-fdivp.md) notes that narrowing on
*assignment* is free (`fst` with no extra slot); this entry is the case where the
narrowing is a real source cast in the middle of the tree and therefore costs a
frame slot.
