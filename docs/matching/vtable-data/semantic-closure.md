# Vtable data semantic closure

This batch closed the last reconstructed data residuals without adding any
`DATA_COMPGEN` declarations. The source already supplies semantic vtable
identity through `VTBL(...)`; candidate COFF supplies slot order and target
identity. The remaining differences were comparison/delink metadata.

## Missing stripped-image relocation sites

The candidate vtables for `soundManager`, `highScoreManager`, and `swapManager`
each contain the ordered slots `Open`, `Close`, and `Main`. The generic retail
relocation sweep recovered the latter two sites but omitted the isolated first
code pointer. Raw candidate and target relocation tables proved the missing
sites independently:

- `0xea954`: `soundManager::Open`
- `0xea630`: `highScoreManager::Open`
- `0xea86c`: `swapManager::Open`

They are reviewed in `config/delink_reloc_inclusions.tsv`; `homm2 audit
reloc-sweep --write` regenerated `config/delink_relocs.tsv` with three new and
zero lost sites. After `homm2 redelink && homm2 build`, all three vtables were
byte-exact with all three ordered `.rdata` relocations matched. This advanced
data from 291,784 / 291,976 bytes (99.934%, 77 / 97 reconstructed data-bearing
units exact) to 291,824 / 291,976 (99.948%, 80 / 97 exact).

## Weak-external deleting destructors

Fourteen remaining units differed only because VC6 records the virtual
vector-deleting destructor `??_E<Class>@@UAEPAXI@Z` as a COFF weak external.
Its `AuxWeakExternal` record names the scalar-deleting destructor
`??_G<Class>@@UAEPAXI@Z` as the default, while the same object defines that
default. The linked retail image therefore contains `??_G`, and the delinked
target correctly names the resolved symbol directly.

The comparison-copy normalizer now applies the same semantic rule as the local
Gruntz matcher: it retargets each weak relocation to its auxiliary default and
then to a same-name definition when present. A fail-closed postcondition permits
only those relocation-index changes. A whole-link build prerequisite scans all
199 candidate and delinked comparison inputs and rejects the transform if any
weak name also has a strong definition. The current corpus proves 15 distinct
weak names and no collision. Regression tests cover both default retargeting
and rejection of a strong override.

This matching-neutral normalization closed 124 bytes, reaching 291,948 /
291,976 (99.990%, 94 / 97 exact), without changing source or linked objects.

## Abstract purecall slots

The final 28 bytes were the vtables for abstract `baseManager`, `widget`, and
`resource`. Their candidate slots all name `__purecall`, and the retail cells
all hold `0x004d9061`. The target previously described that address as
`__strnicmp+0x101` because `__purecall` was absent from the reviewed CRT roster.

Retail RVA `0xd9061` is a nine-byte body byte-identical to VC6
`purevirt.obj::__purecall`:

```asm
push 0x19
call __amsg_exit
pop ecx
ret
```

The sole `REL32` relocation also resolves to the already reviewed
`__amsg_exit` at RVA `0xd8646`. Adding this archive-proven function identity to
`config/crt_functions.csv` made the delinker emit `__purecall+0` at every slot.
Focused `homm2 data-relocs` reports then showed zero `.rdata` residuals and
ordered matches of 3/3, 3/3, and 1/1 for `BASEMGR`, `WIDGET`, and `RESOURCE`.

Final result: 291,976 / 291,976 reconstructed data bytes (100.000%, 97 / 97
data-bearing units exact), with all 1,727 reconstructed functions still exact.
The target-only `(libcmt)` band remains the separately reported external
toolchain carve-out and is excluded from these reconstruction denominators.
