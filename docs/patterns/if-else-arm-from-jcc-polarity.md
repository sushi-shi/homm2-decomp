# The `if/else` arm order is readable straight off the `jcc`

## Trigger

A two-arm `if/else` whose arms are semantically interchangeable (two calls that
differ only in one argument). The fuzzy score stays high because both arms are
present, but the blocks are swapped and every `jcc` is mirrored.

## The rule

MSVC `/Od` emits, for `if (cond) T; else E;`:

```
  <cond>
  jcc_NOT_cond  L_else
  T
  jmp L_end
L_else:
  E
L_end:
```

So the emitted `jcc` is the **negation** of the source condition, and the
**fall-through block is the then-arm**. Reading `jg` therefore pins the source
to `<=`, not `>`.

## Retail vs ours

`?IconToBitmap@@YIXPAVicon@@PAVbitmap@@HHHHHHHHH@Z` (RVA 0xc14c0), solid-fill
clip arm.

retail:

```
25c: 8b 0d ...          movl  s_x, %ecx
262: 03 0d ...          addl  s_run, %ecx
268: 3b 0d ...          cmpl  s_clipR, %ecx
26e: 7f 26              jg    0x296            <-- else
270: <memset(s_row + s_x, s_color, s_run)>     <-- then (fall-through)
294: eb 2d              jmp   0x2c3
296: <memset(s_row + s_x, s_color, s_clipR - s_x + 1)>
```

ours, from `if (s_x + s_run > s_clipR) memset(clipped); else memset(full);`:

```
...                     cmpl  s_clipR, %ecx
                        jle   <else>           <-- mirrored
                        subl  s_x, %edx        <-- clipped length computed FIRST
                        addl  $0x1, %edx
                        <memset(clipped)>
                        <memset(full)>
```

## What closed it

Invert the source condition so the retail fall-through arm becomes the then-arm:

```c
if (s_x + s_run <= s_clipR)
    memset(s_row + s_x, s_color, s_run);
else
    memset(s_row + s_x, s_color, s_clipR - s_x + 1);
```

Five sites in `IconToBitmap` were mirrored this way; fixing all five took it
from a structurally-aligned residual to exact. Note that a bare `if` with no
else keeps the natural polarity (`if (s_x + s_dimLen > s_clipR) s_dimLen = ...;`
emits `jle` over the assignment) - only the two-arm form flips.
