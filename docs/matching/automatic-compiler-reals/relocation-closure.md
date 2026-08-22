# Automatic compiler-real attribution

## Scope

This batch extends the source-free compiler-data policy used for strings to
VC6 `__real@4@...` and `__real@8@...` COMDATs. It follows the Gruntz model:
candidate COFF supplies physical identity, width, topology, and owning
compiland; retail bytes and reviewed relocation sites supply placement. A
`DATA_COMPGEN` annotation is reserved for an actual ambiguity or a semantic
source-owned name, not required for every compiler-emitted constant.

The adapter admits a real literal by either of two fail-closed proofs:

1. Equal function-relative candidate/retail DIR32 evidence places the
   candidate symbol at one retail RVA, with exact payload and storage.
2. Without direct evidence, one fixed-width candidate payload may bind to one
   compatible reviewed retail relocation target.

The VC6 symbol spelling fixes the physical width. This matters for values such
as `12.5` and `180.0`, whose little-endian double payload starts with four zero
bytes and cannot be classified safely by a four-byte content prefix.
Same-spelled external COMDATs are projected into every emitter only when their
complete candidate section signature and checksum agree. Any symbol already
owned by a source `DATA_COMPGEN` claim is reserved globally and remains under
that semantic identity.

The disposable data-symbol normalizer consumes these reviewed physical rows as
well. A delinked `const_*` definition at the exact section coordinate is renamed
to the corresponding `__real@...` identity. Its pre-existing undefined
`__real@...` code-reference symbol may retain a second COFF symbol-table index;
both names are canonical and the section payload, relocation indices, and
relocation addends remain unchanged. Synthetic identities are not exempted
from the audit.

## Measured result

The regenerated manifest enrolled 43 physical real-literal emitters across 10
translation units. CMBTMGR contributed seven previously unmodeled constants:

- `75.0f`, `15.0f`, `78.0f`, `25.0f`, and `150.0f` as four-byte cells;
- `12.5` and `180.0` as eight-byte cells.

All seven use direct relocation/addend evidence. The delinked CMBTMGR object no
longer contains the synthetic 40-byte `.rdata` tail; its twelve native real
COMDAT sections now reproduce the candidate topology, and normalized `.rdata`
is exact at 80 / 80 bytes. The remaining CMBTMGR data deficit is its independent
main `.data` ownership problem; nine `.CRT$XCU`/`.xdata$x` relocation residuals
are compiler-metadata topology and contain no synthetic-identity policy errors.

Eight translation units became data-exact from the shared inference:
`BASE/Ripple`, `SOURCE/AI`, `SOURCE/ARMY`, `SOURCE/COMMAND`, `SOURCE/GAME`,
`SOURCE/Overview`, `SOURCE/SPELLAI`, and `SOURCE/SPELLS`.

Campaign-wide data comparison moved from 287,866 / 292,220 bytes (98.510%,
62 / 97 data-bearing units exact) to 288,742 / 291,988 bytes (98.888%, 70 / 97
units exact). The denominator fell because Vostok stopped materializing 232
bytes of synthetic fallback copies. All 1,727 reconstructed functions remained
exact.

## Verification

- strict repository-wide source-manifest generation with zero diagnostics;
- focused manifest, canonical-data, topology, relocation, and provenance tests;
- forced `homm2 redelink` followed by `homm2 build`;
- normalized CMBTMGR relocation audit: zero policy/provenance/parse errors;
- CMBTMGR `.rdata` exact at 80 / 80 bytes and no synthetic tail section;
- `git diff --check`.
