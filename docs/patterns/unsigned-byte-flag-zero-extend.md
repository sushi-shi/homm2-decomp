# unsigned-byte-flag-zero-extend

**Symptom.** One instruction differs: retail spends two instructions
zero-extending a one-byte member into a dword before testing it, where our
source's `char` member produces a single `movsbl`.

`dropListWidget::DrawDropStuff` (0x4d08d0) was byte-identical for 0x2ee bytes
and then diverged on exactly this, five times over.

## What the bytes say

```
retail                                      ours (member declared `char`)
------------------------------------------- -------------------------------------------
8b 4d f4    movl -0xc(%ebp), %ecx           8b 4d f4    movl -0xc(%ebp), %ecx
33 d2       xorl %edx, %edx                 0f be 91 ac 00 00 00  movsbl 0xac(%ecx), %edx
8a 91 ac 00 00 00  movb 0xac(%ecx), %dl
85 d2       testl %edx, %edx                85 d2       testl %edx, %edx
74 0d       je   <else>                     74 0d       je   <else>
```

`xor r32,r32` + `mov r8` is `/G5`'s zero-extension idiom (VC6 avoids `movzx` on
Pentium). It is emitted only for an **unsigned** one-byte value promoted to
`int`. A `char` member (signed under MSVC's default) gives `movsbl` instead, and
a `bool` member gives a byte-width `test`, not the 32-bit one.

## What made it match

Declare the flag member `u8`, not `char`:

```cpp
    u8 m_scrollUpPressed;
    u8 m_scrollDownPressed;
    u8 m_scrollThumbDragging;
    u8 m_itemSelectionTracking;
```

`DrawDropStuff` 83.31% -> EXACT (and the same four fields carry the shape
through `ProcessSelectDialog`, which reads all of them in `if`/`||` chains).

The type change is layout-neutral — the class size and every other offset are
unaffected — so it is safe to make from a single one-instruction divergence.
