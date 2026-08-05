# `switch` copies its selector to a temp below the register spills

**Trigger.** A short compare chain against consecutive small constants, where
every `cmp` reads **one frame slot that nothing else in the function touches**,
and the chain ends in an unconditional `jmp`. That slot is deeper than the
`this`/`__fastcall` spill, so no local rename can produce it.

An `if`/`else if` chain compares the operand *in place* — the parameter's own
`[ebp+N]` or the local's own slot — and has no trailing `jmp`, because its last
arm's failure simply falls through.

## Probe (VC6 SP5, `/nologo /c /Od /MT /Gr /G5 /Ob1 /Gf /Gi- /GX /DNO_STRICT`)

```cpp
void qA(int align, int w, int lw, int* out) {          // if / else if
    if      (align == 0) *out = 0;
    else if (align == 1) *out = (w - lw) / 2 + 1;
    else if (align == 2) *out = w - lw;
}
void qB(int align, int w, int lw, int* out) {          // switch
    switch (align) {
    case 0: *out = 0;                 break;
    case 1: *out = (w - lw) / 2 + 1;  break;
    case 2: *out = w - lw;            break;
    }
}
```

```
qA  (if / else if)   frame sub esp,8         qB  (switch)   frame sub esp,0xc
-------------------------------------------  -------------------------------------------
                                             8b 45 fc     movl  -0x4(%ebp), %eax
                                             89 45 f4     movl  %eax, -0xc(%ebp)  <- selector temp
83 7d fc 00  cmpl $0x0, -0x4(%ebp)           83 7d f4 00  cmpl  $0x0, -0xc(%ebp)
75 0b        jne  <next test>                74 0e        je    <case 0>
   ...arm 0..., eb 2c jmp <end>              83 7d f4 01  cmpl  $0x1, -0xc(%ebp)
83 7d fc 01  cmpl $0x1, -0x4(%ebp)           74 13        je    <case 1>
75 15        jne  <next test>                83 7d f4 02  cmpl  $0x2, -0xc(%ebp)
   ...arm 1..., eb 11 jmp <end>              74 22        je    <case 2>
83 7d fc 02  cmpl $0x2, -0x4(%ebp)           eb 2b        jmp   <end>             <- default
75 0b        jne  <end>                         ...arm 0..., eb 20 jmp <end>
   ...arm 2... (falls through)                  ...arm 1..., eb 0b jmp <end>
                                                ...arm 2... (falls through)
```

Three separate signatures, any one of which settles the question:

1. `sub esp` is one dword larger for the same locals — the selector temp.
2. `jcc` polarity: the chain jumps **to** the taken arm (`je`) instead of
   **past** the failing test (`jne`).
3. The chain's tail is an explicit `jmp` to the default/end.

The bodies come *after* the whole chain in a `switch`, interleaved with the
tests in an `if` chain; the last arm falls through in both.

## Retail, `font::DrawBoundedString` (RVA 0xc4b70, body 0x1673)

`this` spills to `-0x44(%ebp)`; the selector temp is `-0x48(%ebp)`, one dword
past it — the back-end temp region.

```
1673: 8b 45 20              movl  0x20(%ebp), %eax     ; the `align` parameter
1676: 89 45 b8              movl  %eax, -0x48(%ebp)
1679: 83 7d b8 00           cmpl  $0x0, -0x48(%ebp)
167d: 74 0e                 je    0x168d               ; FONT_ALIGN_LEFT
167f: 83 7d b8 01           cmpl  $0x1, -0x48(%ebp)
1683: 74 11                 je    0x1696               ; FONT_ALIGN_CENTER
1685: 83 7d b8 02           cmpl  $0x2, -0x48(%ebp)
1689: 74 1e                 je    0x16a9               ; FONT_ALIGN_RIGHT
168b: eb 25                 jmp   0x16b2               ; no default arm
```

Ours read the parameter directly (`cmpl $0x2, 0x20(%ebp); jne ...`) and had no
`-0x48` slot at all.

## What made it match

```cpp
switch (align) {
case FONT_ALIGN_LEFT:
    xPosition = 0;
    break;
case FONT_ALIGN_CENTER:
    xPosition = (w - lw) / CENTER_DIVISOR + 1;
    break;
case FONT_ALIGN_RIGHT:
    xPosition = w - lw;
    break;
}
```

`font::DrawBoundedString` -> EXACT.
