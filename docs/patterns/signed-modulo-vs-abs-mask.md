# signed-modulo-vs-abs-mask

**Symptom.** An index into a small power-of-two table. Ours computes `abs(x)`
into a frame temp and masks it; retail has no temp at all and a five-byte AND
with a strange constant.

    ours   cRandomSignText[(x < 0 ? -x : x) & 3]
    ---------------------------------------------------------------------
    4e36   83 7d 0c 00        cmpl  $0x0, 0xc(%ebp)
    4e3a   7d 0d              jge   0x4e49
    4e3c   8b 55 0c           movl  0xc(%ebp), %edx
    4e3f   f7 da              negl  %edx
    4e41   89 95 a0 fc ff ff  movl  %edx, -0x360(%ebp)     <- an ours-only temp
    4e47   eb 09              jmp   0x4e52
    4e49   8b 45 0c           movl  0xc(%ebp), %eax
    4e4c   89 85 a0 fc ff ff  movl  %eax, -0x360(%ebp)
    4e52   ...
    4e86   8b 95 a0 fc ff ff  movl  -0x360(%ebp), %edx
    4e8c   83 e2 03           andl  $0x3, %edx

    retail cRandomSignText[x % 4]
    ---------------------------------------------------------------------
    4ed3   8b 45 0c           movl  0xc(%ebp), %eax
    4ed6   25 03 00 00 80     andl  $0x80000003, %eax      <- keep sign + low bits
    4edb   79 05              jns   0x4ee2
    4edd   48                 decl  %eax
    4ede   83 c8 fc           orl   $-0x4, %eax
    4ee1   40                 incl  %eax

**The rule.** `and imm32` with the **sign bit set** (`0x8000000N`) followed by
`jns / dec / or -(N+1) / inc` is MSVC's inline **signed `% (N+1)`** for a
power-of-two divisor. It is one flat expression: no branch that writes a frame
slot, no temp.

A frame temp plus a `neg`/`jge` pair before a plain `and` is `abs()` (an inline
function, so its argument gets a slot) — a different source expression, and it
also costs a frame slot that retail does not have.

So `and $0x8000000N` in an index expression means the source wrote `x % (N+1)`,
even when the value can only ever be non-negative.

**Closes.** `advManager::DoEvent`, both sign cases (`MAP_OBJECT_BOTTLE` and
`MAP_OBJECT_SIGN`): `cRandomSignText[(x < 0 ? -x : x) & SIGN_RANDOM_TEXT_MASK]`
-> `cRandomSignText[x % SIGN_RANDOM_TEXT_COUNT]`, which also removed two
ours-only frame temps.
