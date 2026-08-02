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
