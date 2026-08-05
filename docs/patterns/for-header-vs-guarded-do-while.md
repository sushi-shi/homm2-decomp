# A guarded `do/while` costs a top guard AND a hoisted running-sum slot

Companion to [for-increment-block-vs-while-tail](for-increment-block-vs-while-tail.md),
which settles `for` vs `while` from the step block. This entry covers the third
spelling — `i = 0; if (limit > i) { do { ... } while (i < limit); }` — and the
declaration defect that always travels with it.

**Trigger.** Retail's loop entry is `init; jmp <test>` with the increment block
parked between them (the `for` skeleton), and retail's frame is exactly one slot
smaller than ours. Ours opens the loop with a compare-and-skip guard, keeps a
running accumulator across iterations, and tests at the bottom.

The guarded `do/while` is not just a skeleton variant: it exists in a
reconstruction precisely *because* someone hoisted a per-iteration product into a
running sum, and that sum needs its own frame slot. Removing the accumulator and
recomputing the product inside the body is what restores retail's `sub esp`.

## The divergence

`BlitBitmapToScreenVesa` (RVA 0xd4610), the row-copy loop. Retail `sub esp,0x14`
(a `RECT` at `-0x10..-0x1` plus `row` at `-0x14`); ours `sub esp,0x18`, with
`destinationOffset` at `-0x18`.

```
retail (exact)                                   ours (82.94%)
------------------------------------------------ ------------------------------------------------
13: c7 45 ec 00.. movl $0x0, -0x14(%ebp) ; row=0 13: c7 45 ec 00.. movl $0x0, -0x14(%ebp)
1a: eb 09         jmp  0x25 <test>               1a: 8b 55 18      movl 0x18(%ebp), %edx ; height
1c: 8b 55 ec      movl -0x14(%ebp), %edx  <inc>  1d: 3b 55 ec      cmpl -0x14(%ebp), %edx
1f: 83 c2 01      addl $0x1, %edx                20: 7e 62         jle  0x84            ; top guard
22: 89 55 ec      movl %edx, -0x14(%ebp)         22: 8b 45 20      movl 0x20(%ebp), %eax ; destY
25: 8b 45 ec      movl -0x14(%ebp), %eax  <test> 25: 69 c0 80 02.. imull $0x280, %eax, %eax
28: 3b 45 18      cmpl 0x18(%ebp), %eax          2b: 89 45 e8      movl %eax, -0x18(%ebp) ; accum
2b: 7d 48         jge  0x75                      2e: ...body...
2d: ...body...                                   68: 8b 45 e8      movl -0x18(%ebp), %eax
    ; (destinationY + row) * 640 recomputed      6b: 05 80 02..    addl $0x280, %eax
    ; inside the body every iteration            70: 89 45 e8      movl %eax, -0x18(%ebp)
73: eb a7         jmp  0x1c <inc>                73..82: increment row, cmp, jl <body>
```

## What made it match

```cpp
// ours, 82.94%
i32 row = 0;
if (height > row) {
    i32 destinationOffset = destinationY * VESA_SCREEN_WIDTH;
    do {
        memcpy(... + destinationX + destinationOffset,
               sourceBitmap->m_pixels + sourceX + (sourceY + row) * sourceBitmap->m_width, width);
        destinationOffset += VESA_SCREEN_WIDTH;
        ++row;
    } while (row < height);
}

// retail, EXACT
i32 row;
for (row = 0; row < height; row++)
    memcpy(
        gpWindowManager->m_screen->m_pixels
            + (destinationY + row) * VESA_SCREEN_WIDTH + destinationX,
        sourceBitmap->m_pixels + (row + sourceY) * sourceBitmap->m_width + sourceX,
        width
    );
```

Two per-site rules travelled with it and are worth reading off any such loop:

- both pointer sums put the **product before the scalar offset**
  (`m_pixels + <product> + destinationX`, not `m_pixels + destinationX +
  <product>`) — [od-add-operand-evaluation-order](od-add-operand-evaluation-order.md).
  The product needs a register so it is emitted first either way; only the
  accumulator (the pointer) names the source's left operand.
- the containing guard flipped to `sourceBitmap != gpWindowManager->m_screen`
  — [integer-relational-operand-side](integer-relational-operand-side.md). Retail
  loads the global into `eax` first *because the right operand needs the base
  register for its address form*, then the LEFT operand into `ecx`, and compares
  `cmpl 0x46(%eax), %ecx`.
