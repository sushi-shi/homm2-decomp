# `BASE/INPUTMGR` data placement closure

## Residual and evidence

`BASE/INPUTMGR` had all 18 functions exact and 208 / 228 data bytes matched.
The entire residual was its 20-byte `.bss` section. Candidate and target
extents already agreed, but `iCurSwapPalette` lacked a Buka `DATA` claim.

The candidate defines the four-byte global at `.bss+0x4`. It lies immediately
after anchored `iLastBWOnScreenCheck` at `.bss+0x0`, RVA `0x00536074`, and
immediately before anchored `bLastMouseOffscreen` at `.bss+0x8`, RVA
`0x0053607c`. Both neighbor spacing and the candidate COFF offset therefore
prove `iCurSwapPalette` at `0x00536078`.

## Result

The retained reconstruction adds the ordinary placement claim:

```cpp
DATA(0x00536078) i32 iCurSwapPalette = 0;
```

After forced `homm2 redelink` and `homm2 build`:

- `BASE/INPUTMGR` is 228 / 228 total data bytes exact;
- `.CRT$XCU` is 4 / 4, `.bss` is 20 / 20, `.data` is 192 / 192, and
  `.rdata` is 12 / 12;
- candidate and target both define `iCurSwapPalette` at `.bss+0x4` with
  external storage;
- all 18 / 18 functions remain exact;
- campaign-wide data comparison moves from 291,764 / 291,976 bytes (99.927%,
  76 / 97 data-bearing units exact) to 291,784 / 291,976 bytes (99.934%,
  77 / 97 units exact).

The focused relocation audit has zero policy, provenance, and parse errors and
three modeled data relocations exact. Its sole diagnostic is the pre-existing
`.CRT$XCU` owner-boundary presentation difference.

## Verification

- forced `homm2 redelink` after adding the source placement claim;
- `homm2 build` and refreshed `homm2 status`;
- normalized `.bss` section-size, symbol-offset, and storage-class comparison;
- focused `homm2 data-relocs --homm2-root .` review;
- `python3 -m unittest scripts.homm2.build.test_annotated_data`;
- `git diff --check`.
