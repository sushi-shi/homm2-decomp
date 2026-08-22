# Automatic compiler-string attribution

## Scope

This batch replaces per-use-site string `DATA_COMPGEN` work with the source-free
attribution policy used by Gruntz. Candidate COFF still supplies the physical
definition and owning compiland; the retail executable and the reviewed HIGHLOW
site manifest remain the address authorities.

The adapter admits a compiler string by either of two proofs:

1. Equal function-relative candidate/retail DIR32 sites plus the candidate COFF
   addend resolve the definition to one retail RVA, and its complete payload and
   storage agree there.
2. As a fallback, one complete NUL-terminated candidate payload in the owning TU
   matches one compatible reviewed retail relocation target.

Repeated candidate payloads and repeated retail RVAs are withheld from the second
path. A direct site/addend proof may disambiguate them. Explicit `DATA_COMPGEN`
bindings reserve their candidate coordinate and remain the escape hatch for a
genuinely ambiguous or non-string allocation.

## Measured result

The regenerated manifest enrolled 1,103 source-free string definitions across 32
units:

- 1,099 by aligned relocation/addend proof;
- 4 by unique relocation-target payload;
- 0 arbitrary order or repeated-payload assignments.

All 1,727 reconstructed functions remained exact. Data comparison moved from
258,597 / 296,655 bytes (87.171%, 41 / 97 data-bearing units exact) to
275,811 / 292,240 bytes (94.378%, 55 / 97 units exact). The denominator fell
because Vostok no longer emits four-byte fallback identities for the admitted
private strings.

`BASE/Misc` was the focused validation unit. Its 142 previously unmodeled initialized
strings are now source-free rows; its existing explicit compiler-data claims remain
reserved. The normalized object is exact in every modeled data class:

- `.CRT$XCU`: 4 / 4 bytes;
- `.bss`: 73 / 73 bytes;
- `.data`: 3,344 / 3,344 bytes;
- total data: 3,421 / 3,421 bytes.

The linked-PE audit reports zero `BASE/Misc` placement, ordered-identity,
multiset-identity, and unresolved-identity divergences. Campaign-wide it reports zero
all-function multiset identity divergences and zero relocation-shape divergences.

## Residual classification

The raw `BASE/Misc` whole-object data-relocation report remains nonzero for five
pre-existing sites and three policy diagnostics which are outside automatic strings:

- one `.CRT$XCU` initializer owner-label classification;
- one raw `$SG56843` versus explicit semantic `cdTrackProbeName` spelling at the same
  `.data` owner/site/type/addend;
- three exception `.xdata$x` relocation-topology sites absent from the delink target;
- two code references to the existing synthetic `const_000ea010` identity.

These residuals do not alter the exact `.data`/`.bss` payload result and remain visible
for their own topology and synthetic-identity cleanup. The global linked-placement
audit still has upstream section-layout, real-literal, and import-cell work; this batch
does not claim linked-executable closure.

## Verification

- focused adapter, canonicalization, reviewed-data, census, and relocation-topology
  unit tests;
- forced `homm2 redelink` with the pinned Vostok delinker;
- `homm2 build` and refreshed `homm2 status`;
- `homm2 data-relocs` for `BASE/Misc` with full HoMM2 provenance checking;
- `python3 -m homm2.build.assert_relocs --pe-data` and focused `BASE/Misc` extraction;
- `git diff --check`.
