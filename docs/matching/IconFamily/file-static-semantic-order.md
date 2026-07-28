# Icon-family file-static semantic order

The 2026-07-28 family pass moved the remaining eight decoder scratch bundles
from function scope to file scope. `IconToBitmapYModify` and
`FlipIconToBitmapYModify` were already file-static. Every `DATA(...)` address,
type, name, and owning TU stayed unchanged.

The order is the union of the historical exact `IconToBitmapYModify` order and
the flipped-only roles, filtered to the state each dialect owns:

1. clip bottom;
2. pixel-loop counter and forward pitch;
3. current Y and X;
4. dim destination;
5. left and optional right boundary;
6. current run and dim palette;
7. row and destination;
8. source skip or literal source, then source cursor;
9. frame entry and span count;
10. fill color, dim length, and clip right.

This is a semantic source convention, not an inferred retail address order.
The retail addresses attached to `DATA(...)` remain authoritative and were
not sorted or rewritten.

## Clean island movement

The scope/order change was intentionally accepted as a new structural parent,
not greedily selected by fuzzy score. The first normalized rebuild changed the
clean islands as follows:

| Function | Before | After |
|---|---:|---:|
| `IconToBitmap` | 76.50% | 74.57% |
| `FlipIconToBitmap` | 85.42% | 85.18% |
| `IconToBitmapColorTable` | 81.30% | 81.63% |
| `MonoIconToBitmap` | 78.54% | 81.05% |
| `DimIconToBitmap` | 73.64% | 78.30% |
| `FlipIconToBitmapColorTable` | 81.23% | 81.63% |
| `FlipMonoIconToBitmap` | 71.89% | 70.51% |
| `FlipDimIconToBitmap` | 64.25% | 67.51% |

Five functions opened higher clean islands and three moved lower. Neither
outcome proves or disproves the storage model by itself. The historical exact
Y-modify evidence and the current family consistency justify retaining the
scope change; each new effective-source hash now receives its own topology and
TU-state search.

`IconToBitmapYModify` itself changed effective source earlier in the semantic
campaign. Its old exact hash `15fda1250f72.852e9521ff86` remains only in the
attempt history. The full build generated the current row at 92.9569% for hash
`05f27a372010.30f5cab9fe5a`; no MAX was copied manually between hashes.

## Required data-topology refresh

Function-static and file-static variables have different private COFF
identities. The first build after the move correctly warned that the fixed
delink model was stale. The validated sequence was:

```sh
python3 -m homm2.build.annotated_functions
homm2 build
homm2 redelink
homm2 build
```

The final build reported the symbol model current, all source `DATA(...)`
definitions valid, all 306,822 data bytes matched, and no relocation-field
review items. This refresh is generated from current source and candidate
objects; no generated manifest or baseline row was hand-edited.
