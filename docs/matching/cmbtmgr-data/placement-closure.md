# CMBTMGR data placement closure

## Evidence

After automatic compiler-real attribution, CMBTMGR's candidate and delinked
`.data` sections both had size 884 and agreed everywhere except offsets
`0x4..0x7`. The candidate bytes were `09 22 56 71`; the delinked bytes were
zero because no reviewed allocation owned that retail cell.

The source object at that exact candidate offset is the existing four-byte
array:

```cpp
u8 wallHex[COMBAT_WALL_SECTION_COUNT] = {9, 34, 86, 113};
```

Retail bytes at RVA `0x004f0c58` are the same four values. The immediately
preceding `giSeed` is already anchored at `0x004f0c54`, and the following
compiler strings begin at `0x004f0c5c`, proving the extent without padding or
overlap. The retained reconstruction therefore adds one ordinary
`DATA(0x004f0c58)` claim to `wallHex`; no compiler-generated identity or raw
offset model is involved.

## Result

CMBTMGR is exact in all modeled data sections:

- `.CRT$XCU`: 4 / 4 bytes;
- `.bss`: 25 / 25 bytes;
- `.data`: 884 / 884 bytes;
- `.rdata`: 80 / 80 bytes;
- total data: 993 / 993 bytes.

The normalized candidate and target `.data` payloads have identical SHA-256
`909f2959f68abaf7de45015505954b72af86d4bbe7123ebcbec1537a69ff909a`.
All 41 CMBTMGR functions remain exact.

Campaign-wide data comparison moved from 288,742 / 291,988 bytes (98.888%,
70 / 97 data-bearing units exact) to 289,626 / 291,988 bytes (99.191%, 71 / 97
units exact).

The strict normalized whole-object relocation audit retains nine independent
`.CRT$XCU`/`.xdata$x` compiler-metadata topology residuals. It reports zero
synthetic-identity policy errors, zero provenance diagnostics, and zero parse
errors; those metadata records do not alter the exact data-byte closure.

## Verification

- forced `homm2 redelink` after adding the source placement claim;
- `homm2 build` and refreshed `homm2 status`;
- exact normalized `.data` byte comparison and SHA-256;
- strict normalized `homm2 data-relocs --homm2-root .` review;
- `git diff --check`.
