# The x87 status-word mask names the source relational (VC6 /Od)

**Symptom.** A float comparison whose `fld`/`fcomp` operands and every
surrounding instruction agree, but `testb $0x41, %ah` faces `testb $0x1, %ah`
(and the following `je`/`jne` flips with it). objdiff scores this as one or two
differing rows; the semantics are identical, so it is invisible to reasoning
about the *meaning* of the condition — only the spelling decides.

After `fcomp`, `ah` carries C3 in bit 6 (0x40, "equal") and C0 in bit 0 (0x01,
"st(0) < operand"). VC6 `/Od` has exactly two lowerings:

| source | emitted |
| :-- | :-- |
| `a < b`  | `fld a; fcomp b; fnstsw; testb $0x1, %ah;  je  <false>` |
| `a <= b` | `fld a; fcomp b; fnstsw; testb $0x41, %ah; je  <false>` |
| `a > b`  | `fld a; fcomp b; fnstsw; testb $0x41, %ah; jne <false>` |
| `a >= b` | `fld a; fcomp b; fnstsw; testb $0x1, %ah;  jne <false>` |

The operands are **never** swapped by the compiler. So `a > b` and `b < a`,
which are semantically the same, produce different bytes: the first loads `a`
and masks 0x41, the second loads `b` and masks 0x01.

## The divergence (`philAI::HeroInteractionAtHero`, RVA 0x86b43)

```
ours  if (armyShare > desiredShare)         retail
--------------------------------------     --------------------------------------
d9 45 f8  flds  armyShare                  d9 45 c4  flds  desiredShare
d8 5d c4  fcomps desiredShare              d8 5d dc  fcomps armyShare
df e0     fnstsw %ax                       df e0     fnstsw %ax
f6 c4 41  testb $0x41, %ah                 f6 c4 01  testb $0x1, %ah
75 ..     jne   <else>                     74 ..     je    <else>
```

Both are `armyShare > desiredShare`. Only

```cpp
    if (desiredShare6 < armyShare7)
```

reproduces retail's `fld desired; fcomp army; test ah,1; je`.

## What made it match

`philAI::HeroInteractionAtHero` has the comparison twice — once choosing the
transfer direction and once inside the non-evaluate arm:

```cpp
    transferFraction3 = (desiredShare6 < armyShare7) ? (armyShare7 - desiredShare6)
                                                     : (desiredShare6 - armyShare7);
    ...
    if (desiredShare6 < armyShare7) { RedistributeTroops(dominant, recipient, ...); }
    else                            { RedistributeTroops(recipient, dominant, ...); }
```

Both sites flipped to `testb $0x1 / je` and the function closed exact.

**Diagnosis rule.** When the only residual rows are `testb $0x41` vs
`testb $0x1` (with or without a `je`/`jne` flip), do not re-derive the
semantics — read which operand retail loads with `fld`, make that the LEFT
operand of the source comparison, and pick the relational from the table above.
