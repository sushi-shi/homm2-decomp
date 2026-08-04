# uchar-cast-widens-param-spill

**Symptom.** A `__fastcall` function whose only parameter is a `char` spills the
whole register (`mov [ebp-4], ecx`, 4 bytes) instead of the byte (`mov [ebp-4], cl`),
and every read of that parameter is a **dword load plus `and 0xff`** rather than a
`movzx`/`movsx` of the byte. Read naively this says "the parameter is an `int`" - and
that reading is wrong, because the call sites pass only `cl`.

## The divergence

`toupper` (RVA 0x67c38) is Buka's CP1251 uppercase fold. Retail:

```
55                pushl  %ebp
8b ec             movl   %esp, %ebp
51                pushl  %ecx
89 4d fc          movl   %ecx, -0x4(%ebp)        <<< dword spill of a char param
8b 45 fc          movl   -0x4(%ebp), %eax
25 ff 00 00 00    andl   $0xff, %eax             <<< dword read + mask
83 f8 61          cmpl   $0x61, %eax
7c 17             jl     ...
```

and every caller passes a byte:

```
8b 55 f4          movl   -0xc(%ebp), %edx
8a 8a <reloc>     movb   0x12616d(%edx), %cl     <<< only cl is set
e8 <reloc>        calll  ?toupper@@YIDD@Z
0f be c0          movsbl %al, %eax
```

Our `char toupper(char c) { if (c >= 'a' && c <= 'z') ... }` gave the byte spill:

```
88 4d fc          movb   %cl, -0x4(%ebp)
0f be 45 fc       movsbl -0x4(%ebp), %eax
83 f8 61          cmpl   $0x61, %eax
```

## What made it match

The parameter type never changes - the **cast in the comparison** does. Probe
(`/Od /MT /Gr /G5 /Ob1 /GX`):

```cpp
char t_char(char c)    { if ((unsigned char)c >= 'a' ...) }  // movl %ecx spill, dword+and
char t_charmask(char c){ if ((c & 0xff)     >= 'a' ...) }    // movb %cl  spill, movsbl
char t_int(int c)      { if ((unsigned char)c >= 'a' ...) }  // identical body to t_char
```

`(unsigned char)c` on a `char` parameter makes VC6 widen the parameter's home slot
to a dword and read it as `dword & 0xff`; `c & 0xff` does not. So the byte-for-byte
body is

```cpp
char toupper(char c) {
    if (static_cast<u8>(c) >= 'a' && static_cast<u8>(c) <= 'z')
        return c - ' ';
    if (static_cast<u8>(c) >= 0xE0 && static_cast<u8>(c) <= 0xFF)   // CP1251 а..я
        return c - ' ';
    if (static_cast<u8>(c) == 0xB8)                                 // ё -> Ё
        return static_cast<char>(0xA8);
    return c;
}
```

44.94% -> EXACT, and the call sites keep their one-byte `mov cl`.

## Corollary: the same shape appears inline

The identical range chain shows up inline in `oldmain`, `game::GetSideDesc` and
`game::ShowCongrats`, with a **byte** parameter temp and a **byte** result temp:

```
8a 91 <reloc>       movb   0x0(%ecx), %dl
88 95 9c fe ff ff   movb   %dl, -0x164(%ebp)     ; inline parameter temp
8b 85 9c fe ff ff   movl   -0x164(%ebp), %eax    ; dword read of that byte slot
25 ff 00 00 00      andl   $0xff, %eax
```

That is an `/Ob1` expansion of a **second, inline** copy of the fold living in a
header - it differs from the out-of-line `toupper` in one detail, the subtraction
operand: inline uses `(u8)c - ' '` (`and edx,0xff; sub edx,0x20`), the out-of-line
`toupper` uses `c - ' '` (`movsbl; sub eax,0x20`). Marking the single `toupper`
`inline` cannot reproduce retail: VC6 then expands it at *every* site, including
`InterpretCommandLine`, which retail calls out of line. Reconstructed as

```cpp
inline char CyrillicToUpper(char c) {          // include/SOURCE/KB.h
    if (static_cast<u8>(c) >= 'a' && static_cast<u8>(c) <= 'z')
        return static_cast<u8>(c) - ' ';
    ...
}
```

`GetSideDesc` 71.69% -> 99.66%, `oldmain` +3.6 points, `ShowCongrats` -> EXACT.
