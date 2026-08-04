# Guard chains: `||` + early return vs `&&` + body

## Trigger

A run of relational tests whose `jcc`s all reach the same place. Two different
source shapes produce it and the bytes tell them apart.

## Shape A - `if (A || B || C || D) return;`

The first N-1 terms take a **short** `jcc` to a shared block; that block is a
single **near** `jmp` to the epilogue; the LAST term is **inverted** and jumps
short over that block into the body.

`?FillBitmapAreaClip@@YIXPAVbitmap@@HHHHHHHHH@Z` (RVA 0xc64e0):

```
9a: 39 55 f8            cmpl %edx, -0x8(%ebp)
9d: 7d 2d               jge  0xcc            ; A true  -> shared block
ac: 7e 1e               jle  0xcc            ; B true  -> shared block
bb: 7d 0f               jge  0xcc            ; C true  -> shared block
ca: 7f 05               jg   0xd1            ; D FALSE -> body (inverted last term)
cc: e9 9b 00 00 00      jmp  0x16c           ; shared block = the `return`
d1: <body>
```

Source:

```c
if (x >= clipx + clipw - 1 || x + w - 1 <= clipx || y >= clipy + cliph - 1
    || y + h - 1 <= clipy)
    return;
```

## Shape B - `if (A && B && C && D) { body }`

All N terms take the SAME `jcc` (each the negation of its source term) to the
join after the body; the body simply follows. No stub, no inversion.

`?IconToBitmap@@YIXPAVicon@@PAVbitmap@@HHHHHHHHH@Z` (RVA 0xc14c0), fill guard:

```
211: 3b 55 1c           cmpl 0x1c(%ebp), %edx
214: 0f 8c 0a 01 00 00  jl   0x324           ; !(s_y >= clipY)
225: 0f 8f f9 00 00 00  jg   0x324           ; !(s_y <= s_clipB)
23a: 0f 8e e4 00 00 00  jle  0x324           ; !(s_x + s_run > clipX)
24c: 0f 8f d2 00 00 00  jg   0x324           ; !(s_x <= s_clipR)
252: <body>
324: <join: s_x += s_run>
```

Source:

```c
} else if (s_y >= clipY && s_y <= s_clipB && s_x + s_run > clipX
           && s_x <= s_clipR) {
    ...
}
```

## What distinguishes them

The stub. Shape A needs one because the `return` target is far and the shared
false-path is a statement (`return;`) rather than a label; shape B's false path
IS the join, so the `jcc`s address it directly - even as near jumps. Writing
shape A as `if (!(A && B && C && D)) return;` also emits the stub, but the
`||` spelling with the terms in retail's order is what reproduced the exact
`jge/jle/jge/jg` sequence.

Both shapes closed on first compile: `FillBitmapAreaClip` 0.00% -> exact,
`IconToBitmap` 29.40% -> exact.
