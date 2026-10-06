# The editor's cell accessor: `cells + x + y * width`

**Trigger.** Map-cell addresses in an editor unit that put the row term
first when a cell is read (`y*width*20`, then `cells + x*20`, then the sum)
but the column term first when the address is kept across a call that
yields the stored value. `fullMap::GetCell` (`&Column(x)[y * width]`) gives
the row-first order in both contexts; the plain expression
`gMap.cells + x + y * gMap.width` gives column-first in both.

Measured on `editManager::PaintRandomTerrain` (EDT2PL.exe 0x1d256):

```
read (CELL_TERRAIN of a cell)            store before the call (cell = Choose(...))
---------------------------------------- ----------------------------------------
mov  edx, [ebp-0x10]  ; y                mov  ecx, [ebp-0x8]   ; x
imul edx, [gMap.width]                   imul ecx, ecx, 0x14
imul edx, edx, 0x14                      mov  edx, [gMap.cells]
mov  eax, [ebp-0x8]   ; x                add  edx, ecx
imul eax, eax, 0x14                      mov  eax, [ebp-0x10]  ; y
mov  ecx, [gMap.cells]                   imul eax, [gMap.width]
add  ecx, eax                            imul eax, eax, 0x14
mov  ax, word ptr [ecx+edx]              add  edx, eax
                                         mov  [ebp-0x54], edx  ; then the call
```

## The probe

An inline accessor whose body is the plain expression,

```cpp
mapCell* CellAt(i32 x, i32 y) { return cells + x + y * width; }
```

reproduces both: `cell = gMap.CellAt(x, y)` and `CELL_TERRAIN(gMap.CellAt(x,
y))` order the row term first, and `gMap.CellAt(x, y)->m_terrainImageIndex =
ChooseGroundTile(...)` computes the column-first address into a temp before
the call. `GetCell` stores row-first, and the open expression in the store
computes the address after the call.

## What made it match

Every cell access of EDITOR/RANDOM spelled through `fullMap::CellAt`.
