# `BASE/WINMGR` data placement closure

## Residual

`BASE/WINMGR` had all 23 functions exact and 72 / 196 data bytes matched.
The entire 124-byte residual was `.bss`: its candidate and delinked extents
were already both `0x7c`, but the ordinary two-byte global `memSelector` had no
Buka `DATA` claim. The missing semantic boundary prevented exact topology for
the whole uninitialized section.

## Placement evidence

The candidate COFF defines `memSelector` at `.bss+0x60`. The preceding
`gCyclePal` is anchored at `0x005348f4` with a proven 96-byte extent, placing
its end and `memSelector` at `0x00534954`. The following function-local static
`savedUpdate` is anchored at `.bss+0x64`, RVA `0x00534958`. This proves the
two-byte `memSelector` cell and the two bytes of alignment padding before
`savedUpdate` without treating padding as a symbol.

## Result

The retained reconstruction adds the ordinary placement claim:

```cpp
DATA(0x00534954) i16 memSelector;
```

After forced `homm2 redelink` and `homm2 build`:

- `BASE/WINMGR` is 196 / 196 total data bytes exact;
- `.CRT$XCU` is 4 / 4, `.bss` is 124 / 124, `.data` is 56 / 56, and
  `.rdata` is 12 / 12;
- candidate and target both define `memSelector` at `.bss+0x60` with external
  storage;
- all 23 / 23 functions remain exact;
- campaign-wide data comparison moves from 291,640 / 291,976 bytes (99.885%,
  75 / 97 data-bearing units exact) to 291,764 / 291,976 bytes (99.927%,
  76 / 97 units exact).

The focused relocation audit has zero policy, provenance, and parse errors.
Its three residual sites are independent compiler metadata: one pre-existing
`.CRT$XCU` owner presentation difference and two candidate-only `.xdata$x` EH
records. Three modeled data relocations match exactly.

## Verification

- forced `homm2 redelink` after adding the source placement claim;
- `homm2 build` and refreshed `homm2 status`;
- normalized `.bss` section-size, symbol-offset, and storage-class comparison;
- focused `homm2 data-relocs --homm2-root .` review;
- `python3 -m unittest scripts.homm2.build.test_annotated_data`;
- `git diff --check`.
