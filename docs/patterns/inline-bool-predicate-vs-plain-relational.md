# A one-line inline `bool` predicate, not a plain relational

**Trigger.** A byte comparison against a literal is materialised as a *value*
before it is branched on:

    xor  r32a,r32a ; mov r8a,[mem]        <- ZERO-extended byte load
    xor  r32b,r32b ; cmp r32a,K ; setcc r8b
    and  r32b,0xff ; test r32b,r32b ; jcc

A plain `if (p[i] == K)` never produces that. At `/Od` a relational used
directly as a condition lowers to `movsbl`/`cmp`/`jcc` — three instructions, no
`setcc`, no mask. The `setcc` + `and 0xff` pair is the `bool` return value of a
**`/Ob1`-expanded inline predicate** being converted back to `int` by the
caller's `if` (`int-cast-suppresses-bool-widen.md` documents the mask itself).

The zero-extended load is the second half of the tell: the argument was
converted to the predicate's **`u8` parameter** on the way in, where a bare
`char` lvalue would have been sign-extended.

## Probe (VC6 SP5, `/nologo /c /Od /MT /Gr /G5 /Ob1 /Gf /Gi- /GX /DNO_STRICT`)

```cpp
static inline bool IsHyphen(u8 c) { return c == '-'; }

void pA(char* text, int pos) { if (text[pos] == '-')      sinkv(); }  // relational
void pB(char* text, int pos) { if (IsHyphen(text[pos]))   sinkv(); }  // predicate
```

```
pA  (plain relational)                       pB  (inline bool predicate)
-------------------------------------------  -------------------------------------------
0f be 08        movsbl (%eax), %ecx          33 c9           xorl  %ecx, %ecx
83 f9 2d        cmpl   $0x2d, %ecx           8a 08           movb  (%eax), %cl
75 05           jne    +5                    33 d2           xorl  %edx, %edx
                                             83 f9 2d        cmpl  $0x2d, %ecx
                                             0f 94 c2        sete  %dl
                                             81 e2 ff 00 00 00  andl $0xff, %edx
                                             85 d2           testl %edx, %edx
                                             74 05           je    +5
```

Both frames are `sub esp,8` — the two `__fastcall` spills only. **The inlined
parameter costs no frame slot**: its one use lets the front end substitute the
argument expression, so the only trace of the call is the `u8` conversion.

## Retail, `font::ExtractLine` (RVA 0xc3b20, body 0xef3)

```
ef3: 8b 55 08              movl  0x8(%ebp), %edx      ; text
ef6: 03 55 f4              addl  -0xc(%ebp), %edx     ; + curPos
ef9: 33 c0                 xorl  %eax, %eax
efb: 8a 02                 movb  (%edx), %al          ; u8 argument
efd: 33 c9                 xorl  %ecx, %ecx
eff: 83 f8 2d              cmpl  $0x2d, %eax
f02: 0f 94 c1              sete  %cl
f05: 81 e1 ff 00 00 00     andl  $0xff, %ecx
f0b: 85 c9                 testl %ecx, %ecx
f0d: 74 28                 je    0xf37
```

Our `if (text[curPos] == '-')` emitted the three-instruction `pA` form at this
site and at the `|| text[curPos] == '-'` term of the hyphenation loop.

## What made it match

```cpp
static inline bool IsHyphen(u8 c) {
    return c == '-';
}
...
if (IsHyphen(text[curPos])) { ... }
```

Two sites converted; `font::ExtractLine` went from 1383 to 0 differing bytes
together with the two other fixes of that pass, and `BASE/FONT` reached 13/13.

## Corollary: how VC6 `/Ob1` spends frame slots on an inlined predicate

`ExtractLine` also expands `IsVowel(u8)` — a 32-term `||` chain — five times.
Those expansions look nothing like `IsHyphen`'s:

```
f3d: 8a 08                 movb  (%eax), %cl
f3f: 88 4d d8              movb  %cl, -0x28(%ebp)     <- parameter slot (front-end region)
f42: 8b 55 d8              movl  -0x28(%ebp), %edx
f45: 81 e2 ff 00 00 00     andl  $0xff, %edx
f4b: 83 fa 61              cmpl  $0x61, %edx          ; 'a'
...
118c: c7 45 c0 00 00 00 00 movl  $0x0, -0x40(%ebp)    <- result slot (BACK-end region)
1193: eb 07                jmp   0x119c
1195: c7 45 c0 01 00 00 00 movl  $0x1, -0x40(%ebp)
119c: 8b 4d c0             movl  -0x40(%ebp), %ecx
119f: 81 e1 ff 00 00 00    andl  $0xff, %ecx
11a5: 85 c9                testl %ecx, %ecx
```

- The parameter is used 32 times, so it gets a real slot; the five expansions
  take `-0x1c, -0x20, -0x24, -0x28, -0x2c` in body order — all **above** the
  `this` spill at `-0x30`, i.e. the front-end inline-expansion region of
  `inline-expansion-slots-after-expression-temps.md`.
- The `bool` result needs a join (the `||` chain), so it is a **back-end**
  temp: `-0x34 .. -0x44`, all *below* `this`.
- `IsHyphen`'s body needs neither: no join, one use — no slot on either side.

**VC6 `/Ob1` emits no `jmp $+0` bracket around these expansions.** The MSVC 4.2
inline fingerprint from the PoL line does not carry over; on this toolchain the
tell is the slot pair above/below the register spill, not a jump-to-next.
