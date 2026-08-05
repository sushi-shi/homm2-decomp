# Index addressing is evidence, not style

`*(mapExtra + x + y * MAP_WIDTH)` looks like something no developer would
write, and it is a standing temptation to "clean up" into `mapExtra[x + y *
MAP_WIDTH]`. **Do not.** The two are not the same expression, and the flat form
is load-bearing at 19 functions and counting.

More generally: at `/Od` the source's address arithmetic survives into the
object almost verbatim, so **three independent axes are byte-visible** — the
grouping, the term order, and whether an inline accessor boundary exists. Each
is therefore *evidence* about what the devs wrote, not a stylistic choice.

## The whole-tree measurement (2026-08-05)

Converting 80 sites across 12 files from `*(base + expr)` to `base[expr]`:

```
functions-exact      1660 -> 1641   (-19)
matched-code-bytes   89.31% -> 82.84%
```

Every one of the 19 regressed from **exactly 100.00%**, none improved:
`GetCloudLookup` 100 -> 73.89, `MapExtraPosAndAdjacentsSet` 100 -> 86.69,
`SetupDynamicStuff` 100 -> 89.67, `SetVisibility` 100 -> 91.24, `DoLichShot`
100 -> 93.03, `VWCompleteDraw` 100 -> 94.20, `DrawDropStuff` 100 -> 95.15,
`SeedPosition` 100 -> 95.64, `TestPossibleDirections` 100 -> 95.93, and ten
more down to `CreateDiffFile` 100 -> 99.81.

## The full spelling matrix

Eight spellings of the same access, measured on `advManager::GetCloudLookup`
(16 sites, the most sensitive function in the set). `col` is the column term,
`row` the row term:

| # | spelling | groups as | inline? | score |
|---|---|---|---|---|
| A | `*(p + col + row*W)` | `(p + col) + row` | no | **100.00%** |
| C | `(p + col)[row*W]` | `(p + col) + row` | no | **100.00%** |
| D | `p[row*W + col]` | `p + (row + col)` | no | 75.29% |
| B | `p[col + row*W]` | `p + (col + row)` | no | 73.89% |
| E | `Acc(col, row)` | `(p + col) + row` | 2-arg | 73.69% |
| H | `AccCol(col)[row*W]` | `(p + col) + row` | 1-arg | 73.69% |
| F | `*(p + row*W + col)` | `(p + row) + col` | no | 73.31% |
| G | `(p + row*W)[col]` | `(p + row) + col` | no | 73.31% |

Read the pairs. **A == C** and **F == G** to the digit: `*` versus `[]` is
provably irrelevant *within one parse tree*. What separates the classes is the
tree, and separately, the inline boundary.

## `*` versus `[]` is not the mechanism — grouping is

`*(E1 + E2)` and `E1[E2]` are the same operation by definition, so subscripting
was never the issue. `+` is left-associative and `/Od` performs no
reassociation, so it emits the address arithmetic **in the order the source
groups it**. `p[col + row*W]` silently regroups `(p + col) + row` into
`p + (col + row)`: the flat form yields the per-term-scaled `[reg+reg]`
addressing retail uses, while the regrouped form computes a whole index first
and then does one add.

## Inline accessors are a third, independent axis

E and H score **identically (73.69%)** and neither reaches A/C, even though H
has the same grouping as A. So the accessor boundary is byte-visible on its own:
MSVC materialises the accessor's return value instead of folding it into the
following address computation. Arity does not matter — a one-term
`AccCol(x)` that merely returns `p + x` costs exactly as much as a two-term
accessor.

**This cuts both ways, and that is the useful part.** Because the boundary is
visible, retail's bytes tell you which form the devs wrote. The tree already
contains both answers:

- `Column(x)[y * width]` (`fullMap::Column` returns `&cells[x]`) — an accessor
  advanced by the column term, subscripted by the row term.
- `Row(y)[x]` (`fullMap::Row` returns `&cells[y * width]`) — the mirror.
- `GetColumn(x)[MAP_WIDTH * y]` in `SOURCE/SEARCH`, `GetRow(y, 1)[MAP_WIDTH *
  p]` in `SOURCE/PHILAI`.

At those sites the accessor **is** the matching form and a flat rewrite would
break it, which is the exact inverse of `GetCloudLookup`. Neither shape is a
hack; each reconstructs a different thing the devs actually typed.

## How to apply

- Never "clean up" `*(base + a + b)` into `base[a + b]`. If readability matters
  at a site, `(base + a)[b]` is byte-identical; `base[a + b]` is not.
- Do not introduce an inline accessor to tidy repeated indexing, and do not
  inline away an existing one, without measuring — the boundary shows.
- Term order inside the index is likewise pinned; see
  `column-accessor-index-order.md`.
- Do not generalise from "semantically equal" to "byte-equal". Measure: one
  `homm2 build` plus a per-function diff of `build/objdiff/report.json` settles
  it permanently. Note `report.json` is **not** a ninja target — a bare `ninja`
  leaves it stale and every variant will read as unchanged.

## Contrast: forms that ARE free

The same whole-tree method showed these move **0 of 2473** rows, and they were
removed as genuine noise (`9c701f2d`): `volatile` on a local, `x | 0`, `x + 0`,
and `(&local)[0]`. Semantic no-ops that do not change grouping, term order, or
an inline boundary are free. Everything else must be measured.
