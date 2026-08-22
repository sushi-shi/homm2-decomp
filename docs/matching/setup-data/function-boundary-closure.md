# SETUP duplicate-string placement closure

## Scope

SETUP contained two distinct local allocations with the same logical payload,
`"x_loadcm.bin"`, at candidate `.data+0x290` and `.data+0x2a0`. Retail likewise
contains two copies at RVAs `0x00116fcc` and `0x00116fdc`. Content alone is a
two-to-two collision, so the automatic string fallback correctly withheld both
instead of assigning occurrences by address order.

The candidate code has one DIR32 reference to each cell inside `game::SetupGame`,
at function-relative offsets `0x314` and `0x3e5`. The reviewed retail relocation
inventory has the same sites and resolves them to the two respective retail
RVAs. This is sufficient occurrence-level placement evidence.

The evidence was previously lost because the candidate relocation walker ended
the function at `$menuDone$56916`, an internal named label at `.text+0x103b`.
COFF identifies it as storage class `Label`, type `Null`; it is not a function
boundary. The walker now terminates only at a later external or static COFF
symbol whose type is `Function`. This is spelling-independent and also handles
named local labels which do not use the usual `$L...` prefix.

Strict manifest generation consequently adds exactly two automatic rows, both
with `candidate-COFF-string:aligned-relocation-addend` provenance. The global
automatic-string inventory moves from 1,103 to 1,105 rows. No SETUP source
annotation or generated compiler-state input is retained.

## Result

The delinked fallback tail is removed and SETUP is exact in all modeled data
sections:

- `.CRT$XCU`: 4 / 4 bytes;
- `.bss`: 9 / 9 bytes;
- `.data`: 903 / 903 bytes;
- total data: 916 / 916 bytes.

The normalized candidate and target `.data` payloads have identical SHA-256
`b78857adc871913dd528effc19ca35eb4027fc8747fa9e4be5c265f1f79859bf`.
All 25 SETUP functions remain exact.

Campaign-wide data comparison moves from 289,626 / 291,988 bytes (99.191%,
71 / 97 data-bearing units exact) to 290,529 / 291,980 bytes (99.503%, 72 / 97
units exact). The denominator falls by eight bytes because the delinker no
longer materializes a synthetic fallback tail.

The strict normalized whole-object relocation audit retains 24 base-only
`.xdata$x` compiler-metadata sites and one `.CRT$XCU` owner-boundary mismatch.
It reports zero synthetic-identity policy errors, zero provenance diagnostics,
and zero parse errors. Those metadata residuals do not alter the exact data-byte
closure.

## Verification

- focused candidate-manifest and data-adapter unit tests;
- strict repository-wide manifest generation with zero diagnostics;
- forced `homm2 redelink` followed by `homm2 build`;
- exact normalized `.data` byte comparison and SHA-256;
- strict normalized `homm2 data-relocs --homm2-root .` review;
- `git diff --check`.
