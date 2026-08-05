# `*(base + x + row)` is a grouping fact, not a codegen trick

`*(mapExtra + x + y * MAP_WIDTH)` looks like something no developer would
write, and it is a standing temptation to "clean up" into `mapExtra[x + y *
MAP_WIDTH]`. **Do not.** The two spellings are not the same expression, and
the flat form is load-bearing at 19 functions and counting.

## Measurement (2026-08-05, whole tree)

Converting 80 sites across 12 files from `*(base + expr)` to `base[expr]`:

```
functions-exact      1660 -> 1641   (-19)
matched-code-bytes   89.31% -> 82.84%
```

Every one of the 19 regressed from **exactly 100.00%**, and none improved:
`GetCloudLookup` 100 -> 73.89, `MapExtraPosAndAdjacentsSet` 100 -> 86.69,
`SetupDynamicStuff` 100 -> 89.67, `SetVisibility` 100 -> 91.24, `DoLichShot`
100 -> 93.03, `VWCompleteDraw` 100 -> 94.20, `DrawDropStuff` 100 -> 95.15,
`SeedPosition` 100 -> 95.64, `TestPossibleDirections` 100 -> 95.93, and ten
more down to `CreateDiffFile` 100 -> 99.81.

## The mechanism is the parse tree, not `[]` versus `*`

`*(E1 + E2)` and `E1[E2]` are the same operation by definition, so the
regression is not about subscripting. It is about **where the parentheses
fall**, because `+` is left-associative:

| spelling | groups as | bytes |
|---|---|---|
| `*(p + x + row)` | `(p + x) + row` | **exact** |
| `(p + x)[row]` | `(p + x) + row` | **exact** |
| `p[x + row]` | `p + (x + row)` | **regresses** |

Proven both directions on `advManager::GetCloudLookup` (the most sensitive
function, -26.11 under the wrong grouping): rewriting its 16 sites to the
`(mapExtra + x)[(y - 1) * MAP_WIDTH]` form moved **0 of 2473** function rows.

`/Od` performs no reassociation, so it emits the address arithmetic in the
order the source groups it. The flat form yields the per-term-scaled
`[reg+reg]` addressing retail uses; the regrouped form computes a whole index
first and then does one add.

## How to apply

- Treat any `*(base + a + b)` in this tree as **retail truth**. It records a
  grouping, and rewriting it silently changes the emitted address arithmetic.
- If readability matters at a given site, `(base + a)[b]` is byte-identical
  and makes the grouping explicit. `base[a + b]` is not.
- The same rule governs term *order* inside the index: see
  `column-accessor-index-order.md`, where retail puts the row product first
  and the base last, and no flat `base + x + W*y` spelling reproduces it.
- Do not generalise from "this expression is semantically equal" to "this
  expression is byte-equal". At `/Od` the source's shape survives into the
  object almost verbatim. Measure.

## Contrast: forms that ARE free

The same whole-tree A/B method showed these move **0 of 2473** rows and were
removed as genuine noise: `volatile` on a local, `x | 0`, `x + 0`, and
`(&local)[0]`. Semantic no-ops that do not change grouping are free; changing
grouping is not.
