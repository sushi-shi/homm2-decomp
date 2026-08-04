# Inline-call operand forces index-first evaluation (VC6 /Od /Ob1)

For a two-term address `*(P + I)` (or `P[I]`), VC6 /Od evaluates the
POINTER-typed operand's subtree first, regardless of source position:

- `(cells + x)[y * width]` and `*(y * width + (cells + x))` both compute
  `x`, load `cells`, add - then fold `y*width` into the addressing mode.

The exception that decides matches: when the pointer side contains an
INLINED accessor call (/Ob1), the plain index side is evaluated FIRST and
the call body second:

- `Row(y)[x]` (Row = `&cells[y * width]`) -> x*scale, then this/y/imul,
  add cells+yw, fold x.
- `Column(x)[y * width]` (Column = `&cells[x]`) -> y*width first, then
  x*scale, cells load, add cells+x, fold yw. This is the only spelling
  that reproduces retail's yw-x-cells-add order with the cells+x base
  (EDITOR/mapcell GetNewCellExtra twins, proven byte-exact 2026-08-02).

In VALUE contexts (assignment, argument) the same tree keeps plain
left-to-right instead: `cell = cells + x + y * width;` emits x, cells,
add, yw, add - two real adds, no addressing fold.

Related: a zero-arm ternary lowers branchless: `t = cond ? v : 0;` emits
sub/neg/sbb/and instead of the if/else two-jmp (ChangeTilesetIndex head).

## Second confirmation (SOURCE/PHILAI, 2026-08-04)

`philAI::GetTurnAIVars` (0x808a1) and `philAI::RVOfPosition` (0x84642) both read
`gpSearchArray`'s node grid. `searchArray::GetRow(i32 y, i32 width)` returns
`m_storage.nodes + y * width` and `searchNode` is 9 bytes, so every term is
scaled and the order is fully visible:

```
ours  GetRow(MAP_WIDTH, y4)[x3]            retail
--------------------------------------     --------------------------------------
8b 4d d4  movl x, %ecx                     8b 0d ..  movl MAP_WIDTH, %ecx
6b c9 09  imull $0x9, %ecx, %ecx           0f af 4d cc imull y, %ecx
8b 15 ..  movl MAP_WIDTH, %edx             6b c9 09  imull $0x9, %ecx, %ecx
0f af 55 cc imull y, %edx                  8b 55 d4  movl x, %edx
6b d2 09  imull $0x9, %edx, %edx           6b d2 09  imull $0x9, %edx, %edx
   ... base + (W*y)*9, SIB index x*9          ... base + x*9, SIB index (W*y)*9
```

The five-arm probe (`b1..b5`, `d1..d4`) separates the classes cleanly: only
`gA->GetRow(x, 1)[gW * y]` puts the row product in the SIB index and the column
in the base accumulator, because the inlined accessor call sits on the pointer
side and the subscript is therefore evaluated first. (The `* 1` disappears -
VC6 folds a unit scale.) The non-call pointer forms `(gA->nodes + x)[gW*y]` and
`*(gA->nodes + x + gW*y)` both evaluate the COLUMN first and do not match.

Closing spelling in both functions:

```cpp
    if (gpSearchArray->GetRow(x3, 1)[MAP_WIDTH * y4].visited) { ... }
    distanceFactor3 = static_cast<float>(gpSearchArray->GetRow(x, 1)[MAP_WIDTH * y].distance)
                      / gpCurAIHero->m_mobility;
```

## The same rule decides a RELATIONAL's operand sides (2026-08-04)

[integer-relational-operand-side](integer-relational-operand-side.md) reads the
answer off which operand reaches its register last. That rule assumes neither
side contains a call. When exactly ONE side holds an inlined accessor, the
call-bearing side is evaluated **last** and the plain side first, while the
`cmp` keeps source order — so the register-load order inverts relative to the
call-free spelling and the naive reading picks the wrong arm.

`advManager::Main` (RVA 0x27cf), the `INPUT_SCAN_T` loop over `m_townIds`.
Four spellings, one matrix, identical semantics:

```
A  TownId(i)      == m_currentTown       B  m_currentTown == TownId(i)
   movsbl currentTown -> %ecx               movsbl currentTown -> %ecx
   movsbl townIds[i]  -> %eax               movsbl townIds[i]  -> %eax
   cmp %ecx, %eax   (3b c1)                 cmp %eax, %ecx   (3b c8)

C  m_currentTown  == m_townIds[i]        D  CurrentTown()  == m_townIds[i]   == retail
   movsbl currentTown -> %ecx               a1 <gpCurPlayer>   movl gpCurPlayer, %eax
   movsbl townIds[i]  -> %eax               03 45 e8           addl -0x18(%ebp), %eax
   cmp %ecx, %eax                           0f be 48 47        movsbl 0x47(%eax), %ecx
                                            8b 15 <gpCurPlayer> movl gpCurPlayer, %edx
                                            0f be 42 45        movsbl 0x45(%edx), %eax
                                            3b c1              cmp %eax, %ecx
```

A, B and C all evaluate `m_currentTown` first: with no call on either side VC6
takes the *simpler* operand first regardless of position. Only D — the inlined
`CurrentTown()` on the LEFT — pushes that read to the end and evaluates
`m_townIds[i]` first, which is retail's order. The accessor is therefore
load-bearing at this site even though an `i32` accessor and a member read are
byte-identical in isolation (see
[inline-accessor-return-width](inline-accessor-return-width.md)).

Two calls, one on each side (`CurrentTown() == TownId(i)`), fall back to the
call-free order; the rule only fires when exactly one side has a call.

