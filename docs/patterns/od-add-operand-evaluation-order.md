# `+` / `*` operand order: foldable right operand vs one that needs a register

## Trigger

Two functions in the same TU build the same pointer with the same three terms
but emit the terms in a different order, or an `imul` picks a different
destination register. Nothing about the values differs - only the source
spelling of the additions.

## The rule (measured, VC6 SP5 /Od)

For a binary `A op B`:

- If `B` is a leaf that can be an x86 memory operand (a plain dword local,
  global, or already-loaded member), MSVC emits `A` first and folds `B` into
  the instruction: `add A_reg, B_mem` / `imul A_reg, B_mem`. **The result lives
  in A's register.**
- If `B` needs its own register (a `movswl` of an `i16` member, a product, a
  parenthesised subexpression), MSVC emits **`B` first**, then `A`, then
  `op A_reg, B_reg`. **The result still lives in A's register.**

So the register that survives always names the source's LEFT operand, and the
code that comes FIRST names the right operand whenever the right operand is not
foldable.

## Retail vs retail (same file, different spelling)

`BASE/bmap2.c.obj`. `FillBitmapArea` (RVA 0xc6450) and `DimBitmapArea`
(RVA 0xc6690) both compute `pixels + x + y*width`, but with different
associations:

```
FillBitmapArea      gFillPtr = bmp->m_pixels + x + y * bmp->m_width;
 f: 8b 48 16        movl   0x16(%eax), %ecx    ; m_pixels          (left of the outer +)
12: 03 4d f8        addl   -0x8(%ebp), %ecx    ; + x               (foldable right -> left first)
18: 0f bf 42 12     movswl 0x12(%edx), %eax    ; m_width           (right of the *, not foldable)
1c: 8b 55 08        movl   0x8(%ebp), %edx     ; y                 (left of the *)
1f: 0f af d0        imull  %eax, %edx          ; edx = y * m_width (result in y's register)
22: 03 ca           addl   %edx, %ecx          ; result in (pixels+x)'s register

DimBitmapArea       gDimPtr = bmp->m_pixels + y * bmp->m_width + x;
23b: 0f bf 48 12    movswl 0x12(%eax), %ecx    ; m_width
23f: 8b 55 08       movl   0x8(%ebp), %edx     ; y
242: 0f af d1       imull  %ecx, %edx          ; product computed FIRST (non-foldable right)
248: 8b 48 16       movl   0x16(%eax), %ecx    ; m_pixels          (left, second)
24b: 03 ca          addl   %edx, %ecx          ; result in m_pixels' register
24d: 03 4d f8       addl   -0x8(%ebp), %ecx    ; + x               (foldable right)
```

Our wrong spelling for the multiply, `bmp->m_width * y`:

```
movsx ecx, word ptr [eax + 0x12]     ; m_width is now the LEFT operand
imul  ecx, dword ptr [ebp + 8]       ; y is foldable -> folded, result in m_width's reg
```

## What closed it

Read the multiply first: `imul reg, mem` means the memory term is the source's
RIGHT operand; `imul rA, rB` means `rB` is the right operand and `rA` holds the
left. Then read the additions the same way. For bmap2 that gave two different
source lines in one file:

```c
gFillPtr = bmp->m_pixels + x + y * bmp->m_width;   /* FillBitmapArea */
gDimPtr  = bmp->m_pixels + y * bmp->m_width + x;   /* DimBitmapArea  */
```

Both went exact. The same reading fixed every `s_row = dest->m_pixels + s_y *
s_pitch;` and `s_entry = reinterpret_cast<IconEntry*>(srcIcon->m_data) + frame;`
in the eleven icon decoders.
