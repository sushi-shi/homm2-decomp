# `movsx r32, ax` after an int `add` — an `i16` cast of the whole sum

**Trigger.** A dword field is written from a value that VC6 computes in 32 bits and
then sign-extends through its own low half:

```
33 c0        xorl  %eax, %eax
83 7d 0c 00  cmpl  $0x0, 0xc(%ebp)
0f 95 c0     setne %al
83 c0 04     addl  $0x4, %eax
0f bf c8     movswl %ax, %ecx        <<< the tell
89 4d e0     movl  %ecx, -0x20(%ebp)
```

`movsx r32,r16` on a value that was already 32-bit means the SUM passed through a
16-bit type. Only an explicit narrowing cast of the whole expression does that; a cast
of either operand alone is absorbed by integer promotion and emits nothing.

Measured on `game::ViewSpells` (RVA 0x546ee), `message.payload.widget.id`.

## The probe (VC6 SP5, `/Od /MT /Gr /G5 /Ob1 /Gi- /GX`)

```cpp
enum ESmall { A4 = 4, A5 = 5 };
enum ST { ST_C = 0, ST_A = 1, ST_ALL = 2 };
struct M { int a; int b; int id; int c; };

void f1(M& m, int st) { m.id = A4 + (st != 0); }                    // and 0xff, no movsx
void f2(M& m, int st) { m.id = (short)(A4 + (st != 0)); }           // and 0xff + movsx
void k3(M& m, ST st)  { m.id = (short)(4 + (st != ST_C)); }         // MATCH
void k4(M& m, ST st)  { m.id = (short)((int)A4 + (st != ST_C)); }   // MATCH
void k2(M& m, int st) { m.id = (short)(A4 + (int)(st != 0)); }      // MATCH
void k6(M& m, ST st)  { m.id = 4 + (st != ST_C); }                  // no movsx
void f8(M& m, int st) { m.id = (char)(4 + (st != 0)); }             // movsbl, not movswl
```

```
f1 / f2 (enum addend)                          k4 (enum addend cast to int)
---------------------------------------------  ---------------------------------------------
33 c0        xorl  %eax, %eax                  33 c0        xorl  %eax, %eax
83 7d f8 00  cmpl  $0x0, -0x8(%ebp)            83 7d f8 00  cmpl  $0x0, -0x8(%ebp)
0f 95 c0     setne %al                         0f 95 c0     setne %al
25 ff 00 00 00  andl $0xff, %eax    <<<
83 c0 04     addl  $0x4, %eax                  83 c0 04     addl  $0x4, %eax
0f bf c8     movswl %ax, %ecx  (f2 only)       0f bf c8     movswl %ax, %ecx
```

The `and $0xff` is the separate [bool-plus-enum-mask](bool-plus-enum-mask.md) finding:
`enumerator + bool` masks the bool, `int + bool` does not. Both effects are live in one
statement here, so both casts are needed.

## What made it match

```cpp
            message.payload.widget.id = static_cast<i16>(
                VIEW_SPELL_COMBAT_TAB_ID
                + static_cast<i32>(spellType != SPELL_TYPE_COMBAT)
            );
```

(previously an `if/else` pair assigning `VIEW_SPELL_COMBAT_TAB_ID` /
`VIEW_SPELL_ADVENTURE_TAB_ID`.) `game::ViewSpells` 96.67% -> EXACT.

The mirrored form — an `i8` cast — emits `movsbl %al, %ecx` instead, so the register
half named in the `movsx` tells you the width directly.
