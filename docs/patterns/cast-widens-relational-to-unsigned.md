# `jb`/`jbe` where retail has `jl`/`jle`: a cast made the comparison unsigned

**Trigger.** Two objects agree instruction for instruction — same loads, same
`and $0xff` zero-extension, same immediate — and differ only in the condition code
of the branch that follows: ours `jb`/`jbe` (unsigned), retail `jl`/`jle` (signed).
Nothing about the *values* differs; a `u32`/`unsigned` cast on one side of the
relational promoted the whole comparison to unsigned.

Measured on `PopNetBox` (RVA 0x6c0a6), the printable-key range guard.

## The probe (VC6 SP5, `/Od /MT /Gr /G5 /Ob1 /Gf /Gi- /GX`)

```cpp
union K { int keyCode; u8 keyByte; };
struct M { K k; };  M g_msg;  int g_len;

void p_unsigned(void) { if (g_msg.k.keyByte < (u32)0x20 || g_msg.k.keyByte > (u32)0x7f) return; g_len = 1; }
void p_signed(void)   { if (g_msg.k.keyByte < 0x20      || g_msg.k.keyByte > 0x7f)      return; g_len = 1; }
```

```
p_unsigned  (ours)                            p_signed  (retail)
--------------------------------------------- ---------------------------------------------
33 c0        xorl  %eax, %eax                 33 c0        xorl  %eax, %eax
a0 00 00 00 00  movb 0x0, %al                 a0 00 00 00 00  movb 0x0, %al
83 f8 20     cmpl  $0x20, %eax                83 f8 20     cmpl  $0x20, %eax
72 0d        jb    L1                         7c 0d        jl    L1
33 c9        xorl  %ecx, %ecx                 33 c9        xorl  %ecx, %ecx
8a 0d ..     movb  0x0, %cl                   8a 0d ..     movb  0x0, %cl
83 f9 7f     cmpl  $0x7f, %ecx                83 f9 7f     cmpl  $0x7f, %ecx
76 02        jbe   L2                         7e 02        jle   L2
```

Byte for byte identical except `72`/`7c` and `76`/`7e`. The `u8` member already
zero-extends (`/G5` AND-not-MOVZX, here the `xor`+`movb` form for a global), so both
spellings are semantically the same for every reachable value — only the source type
of the *other* operand decides the opcode.

## What made it match

```cpp
                        if (event_a.payload.keyboard.keyByte < BOX_FIRST_PRINTABLE
                            || event_a.payload.keyboard.keyByte > BOX_LAST_PRINTABLE)
                            break;
```

(previously `< static_cast<u32>(BOX_FIRST_PRINTABLE)` / `> static_cast<u32>(BOX_LAST_PRINTABLE)`.)
A `u8` and an enumerator both promote to `int`, so the plain comparison is signed.
The cast is invisible in the value domain and visible in exactly two opcode bytes.
