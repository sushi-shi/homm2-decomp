# `fullMap::GetCell(x, y)` vs `Row(y) + x`: which term is added to `cells` first

**Trigger.** Both spellings materialise the same inline `this` temp
(`gpGame + 0xb3e` stored to a frame slot) and both scale by `sizeof(mapCell) == 12`, but
they differ in **which product is computed first and which is added to `cells` first**.

- `Row(y) + x` — `mapCell* Row(i32 y) { return &cells[y * width]; }` — evaluates the
  `+ x` operand first, then the accessor body, and accumulates `cells + (y*width*12) + (x*12)`.
- `GetCell(x, y)` — `mapCell* GetCell(i32 x, i32 y) { return &Column(x)[y * width]; }` —
  evaluates the index `y * width` first, then the nested `Column(x)` accessor, and
  accumulates `cells + (x*12) + (y*width*12)`.

Retail uses the second form. Measured on `advManager::GenericSiteEvent` (RVA 0x42256,
Eye-of-Magi scan) and `advManager::EraseObj` (RVA 0x41609).

## The divergence

```
ours  (Row(y) + x)                            retail  (GetCell(x, y))
--------------------------------------------- ---------------------------------------------
8b 4d ec  movl  -0x14(%ebp), %ecx   ; x       8b 4d b8  movl  -0x48(%ebp), %ecx  ; this
6b c9 0c  imull $0xc, %ecx, %ecx              8b 55 d4  movl  -0x2c(%ebp), %edx  ; y
8b 55 b8  movl  -0x48(%ebp), %edx   ; this    0f af 51 08 imull 0x8(%ecx), %edx  ; y*width
8b 45 d4  movl  -0x2c(%ebp), %eax   ; y       6b d2 0c  imull $0xc, %edx, %edx
0f af 42 08 imull 0x8(%edx), %eax   ; y*width 8b 45 ec  movl  -0x14(%ebp), %eax  ; x
6b c0 0c  imull $0xc, %eax, %eax              6b c0 0c  imull $0xc, %eax, %eax
8b 55 b8  movl  -0x48(%ebp), %edx              8b 4d b8  movl  -0x48(%ebp), %ecx
8b 12     movl  (%edx), %edx        ; cells   8b 09     movl  (%ecx), %ecx       ; cells
03 d0     addl  %eax, %edx          ; + yterm 03 c8     addl  %eax, %ecx         ; + x*12
03 d1     addl  %ecx, %edx          ; + xterm 03 ca     addl  %edx, %ecx         ; + yterm
```

Both forms produce the same address; only the accumulation order and hence the register
assignment differ, so the mismatch propagates into every following statement.

## What made it match

```cpp
    currentCell36 = gpGame->m_worldMap.Row(mapY14) + mapX37;      // ours
```
```cpp
    currentCell36 = gpGame->m_worldMap.GetCell(mapX37, mapY14);   // retail
```

`GenericSiteEvent` was one of the two changes that took it to EXACT; `EraseObj`'s
`cells_h[i] = gpGame->m_worldMap.Row(cellY) + cellX;` needed the same rewrite and went
95.05% -> EXACT.
