# A run of `x = (float)(x * expr);` keeps the running value in `st0`

**Trigger.** An x87 chain that loads a `float` local once, multiplies, stores it
with a NON-popping `fst dword ptr [x]`, and then multiplies again **without
reloading** `x`. It looks like one expression, but it is a run of ordinary
consecutive statements.

Measured on `philAI::ValueOfBuyingBuilding` (RVA 0x83019), two sites.

## The divergence

```
ours   (a + K) * x  per statement                retail
------------------------------------------      ------------------------------------------
8b 15 .. mov  edx, [gpCurPlayer]                 d9 45 fc fld   dword [ebp-0x4]   ; x
d9 82 .. fld  dword [edx+0xbf]                   8b 15 .. mov   edx, [gpCurPlayer]
dc 05 .. fadd qword [0.66]                       d9 82 .. fld   dword [edx+0xbf]
d8 4d c0 fmul dword [ebp-0x40]   ; * x           dc 05 .. fadd  qword [0.66]
d9 5d c0 fstp dword [ebp-0x40]   ; store, pop    de c9    fmulp  st(1), st
                                                 d9 55 fc fst   dword [ebp-0x4]  ; store, KEEP
a1 .. .. mov  eax, [gpCurPlayer]                 a1 .. .. mov   eax, [gpCurPlayer]
d9 80 .. fld  dword [eax+0xc3]                   d9 80 .. fld   dword [eax+0xc3]
...      (reloads x again)                       ...      (no reload - st0 still holds x)
```

Retail's `d9 55 fc` is `fst` (store, no pop); ours is `d9 5d c0` `fstp`.

## What made it match

Two independent axes, both required:

1. the running value must be the **left** operand of each multiply, and
2. the statements stay **separate** - one assignment per line.

```cpp
    /* was: adjustedValue = (float)((w + 0.66) * adjustedValue);  x3          */
    adjustedValue = static_cast<float>(adjustedValue * (w + 0.66));
    adjustedValue = static_cast<float>(adjustedValue * (u * 2.0f + 0.33));
    adjustedValue = static_cast<float>(adjustedValue * (dwellingTotal * 0.33 + 0.66));
```

VC6 `/Od` recognises that the next statement's first operand is the value it
just stored and skips the reload, emitting `fst` instead of `fstp`.

**Do not** reach for the equivalent nested-cast expression

```cpp
    adjustedValue = static_cast<float>(
        static_cast<float>(static_cast<float>(adjustedValue * a) * b) * c);
```

It produces the *same instruction stream* - the block diff goes fully exact -
but each inner `static_cast<float>` also reserves a 4-byte narrowing temp that
the emitted code never touches, so the frame grows by 4 bytes per cast
(`sub esp,0xa0` instead of retail's `0x90` for two such chains) and every
`-0x..(%ebp)` displacement shifts. The separate-statement spelling reproduces
retail's frame exactly.

`philAI::ValueOfBuyingBuilding` 93.84% -> EXACT.
