# Reviewed external data-alias closure

## Problem

Reviewed relocation aliases name the semantic source object and addend when the
retail operand points before, inside, or beyond a PDB symbol boundary. Vostok
previously classified every such data alias as `ConjuredStatic` or
`ConjuredConstant` before consulting the canonical data manifest. With
materialization enabled, a reference from another translation unit therefore
appended a private copy of the relocation target to that unit.

The clearest instance was `ExpCampaign::IvanName` at RVA `0x0b520b`. Its
relocation at `0x0b522b` is reviewed as `xStableText + 0x18`.
`xStableText` is an external definition owned by `SOURCE/X_GLOBAL` at RVA
`0x11b7c0`, but the old classification appended a four-byte cell to
`SOURCE/X_CAMPGN`. That made the delinked `.data` section `0x576` bytes instead
of the candidate's `0x572` bytes and gave the appended cell the misleading
physical target name `xJosephName2`.

This is an ownership error, not a missing compiler-generated source object.
Adding `DATA_COMPGEN` in `X_CAMPGN`, or excluding the real PE relocation at
RVA `0x11b62c`, would encode the wrong owner.

## Resolution

`patches/vostok-reviewed-static-reuse.patch` now lets the data manifest answer
whether an alias owner is a reviewed external definition. All three alias
recovery paths use `ReviewedData` for such an owner:

- exact PDB instruction operands;
- absolute relocations targeting `.rdata`;
- absolute relocations targeting `.data`.

The reviewed addend is preserved. A `ReviewedData` target is emitted as an
undefined external reference in the consuming object, so the manifest owner
remains the sole definition. Local definitions retain the former conjured
materialization behavior, and the existing same-object reuse path is unchanged.

## Verification

The final patched Vostok derivation passed all 88 upstream unit tests, including
the new external-scope lookup test. A complete `homm2 redelink` followed by
`homm2 build` kept all 1,727 reconstructed functions exact and produced
286,523 / 292,220 exact data bytes (98.050%). Three data-bearing units closed:

| Unit | Exact data | Relevant audited function relocations |
| --- | ---: | --- |
| `SOURCE/HERO` | 1,135 / 1,135 | `0x61cd6`: 8/8; `0x61d60`: 8/8; `0x64a9e`: 11/11 |
| `SOURCE/TOWN` | 8 / 8 | `0xa43f9`: 9/9 |
| `SOURCE/X_CAMPGN` | 1,399 / 1,399 | `0xb520b`: 2/2 |

For `HERO`, the candidate and target `.data` sections are byte-identical at
1,107 bytes. For `X_CAMPGN`, they are byte-identical at 1,394 bytes and the
target now contains an undefined `xStableText` symbol rather than a private
`xJosephName2` definition. `TOWN` correctly has no `.data` section in either
object; its `.CRT$XCU` and `.bss` data account for the exact eight bytes.

A focused `homm2 data-relocs` audit of `X_CAMPGN` reports no policy errors and
no provenance diagnostics. Its five remaining topology diagnostics are the
pre-existing compiler/linker metadata differences in `.CRT$XCU` and
`.xdata$x`, not source data ownership residuals.

## Disposition

Cross-object compiler data is matched by content, placement, and reviewed
relocation semantics. Per-object `DATA_COMPGEN` annotations remain necessary
only when those signals are ambiguous; they are not required for these aliases.
