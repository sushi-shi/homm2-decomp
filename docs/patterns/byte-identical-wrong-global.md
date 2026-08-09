# A 100.00% function can still read the wrong global (VC6 /Od)

**Symptom (historical configuration).** With `functionRelocDiffs: "none"`, the
four bytes of every DIR32 operand were masked on both sides: a function that read
`gArmyNames` where retail read `gArmyNamesPlural` was *byte-identical* and could
report 100.00%. The project now uses `data_value`, which exposes differing
referenced values in the ordinary score. `python3 -m homm2.build.assert_relocs`
remains authoritative because equal-valued storage and owner-relative addends can
still evade value-only comparison.

Two source shapes produce this class, both with **identical instruction bytes**:

| source difference | what moves | what does not |
| :-- | :-- | :-- |
| operand order of a relational between two globals | which global the `mov` loads and which the `cmp` reads | opcodes, ModRM, frame, CFG |
| compound `if ((a = b) != 0)` versus `a = b; if (b != 0)` | which global the final `cmp mem,$0` names | opcodes (`a1`/`a3`/`83 3d`), sizes, jumps |

VC6 `/Od` always loads the **left** operand of a relational first, so `a != b`
and `b != a` name different symbols at the same two sites. MSVC 4.2 does the
same, which is why the PoL 2.0 reconstruction distinguishes the two spellings
in the two copies of this very block.

## The divergence (`CheckChangeCursor`, RVA 0xbd200)

Instruction bytes agree everywhere; only the relocation symbols differ.

```
ours   if (gbColorMice != bLastOnscreenMouseColor)     retail
------------------------------------------------      ------------------------------------------------
8b 15 00 00 00 00  movl 0x0, %edx                      8b 15 00 00 00 00  movl 0x0, %edx
   +2: DIR32 ?gbColorMice@@3HA                            +2: DIR32 ?bLastOnscreenMouseColor@@3HA
3b 15 00 00 00 00  cmpl 0x0, %edx                      3b 15 00 00 00 00  cmpl 0x0, %edx
   +2: DIR32 ?bLastOnscreenMouseColor@@3HA                +2: DIR32 ?gbColorMice@@3HA
74 0d              je   <skip>                         74 0d              je   <skip>
```

```
ours   if ((bLastOnscreenMouseColor = gbColorMice) != 0)   retail
--------------------------------------------------------  --------------------------------------------------------
a1 00 00 00 00       movl 0x0, %eax                        a1 00 00 00 00       movl 0x0, %eax
   +1: DIR32 ?gbColorMice@@3HA                                +1: DIR32 ?gbColorMice@@3HA
a3 00 00 00 00       movl %eax, 0x0                         a3 00 00 00 00       movl %eax, 0x0
   +1: DIR32 ?bLastOnscreenMouseColor@@3HA                    +1: DIR32 ?bLastOnscreenMouseColor@@3HA
83 3d 00 00 00 00 00 cmpl $0x0, 0x0                        83 3d 00 00 00 00 00 cmpl $0x0, 0x0
   +2: DIR32 ?bLastOnscreenMouseColor@@3HA                    +2: DIR32 ?gbColorMice@@3HA
74 0d                je   <skip>                           74 0d                je   <skip>
```

The compound form re-reads the **assigned** object for the test; two statements
re-read the **source** object. Same opcodes, same displacement encoding, same
function size — a different global.

## What made it match

```cpp
        if (bLastOnscreenMouseColor != gbColorMice)
            gpMouseManager->SetColorMice(1);
    } else if (bLastMouseOffscreen == 0) {
        bLastMouseOffscreen = 1;
        bLastOnscreenMouseColor = gbColorMice;
        if (gbColorMice != 0)
            gpMouseManager->SetColorMice(0);
    }
```

`assert_relocs` fell from three rows to zero for this function and objdiff did
not move: 100.00% before, 100.00% after.

**Diagnosis rule.** A relocation-target row on an exact function is never a
codegen puzzle. Pair the two objects' relocation sites by function-relative
offset (`parse_obj(..., with_sites=True)` on both sides) and read the site the
audit names: the retail symbol at that site is the operand the source must load
there. Reach for the source operand order and the assignment/test split before
anything else — the bytes cannot tell you, and the fuzzy score never will.
