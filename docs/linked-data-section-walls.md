# Linked data section walls

The final PE has two independent data results. Reconstructed objects match their
retail data bytes, and the exhaustive final-image code-reference audit resolves
all 22,384 relevant `.rdata`/`.data` operands to the same semantic identities.
The raw linked sections are not exact because absolute addresses and contribution
order encode the unresolved final-link history. This ledger partitions every raw
data-section residual without treating address equality as semantic identity.

## Current raw partition

The section geometry and raw sizes agree. `.rdata` has 3,489 differing bytes and
`.data` has 3,753. The raw-backed BSS prefix, virtual-only BSS extent, alignment
gaps, and section tails are independently exact or loader-zero-equivalent.

| Section band | Differing bytes | Classification |
| --- | ---: | --- |
| `.idata$5` | 375 | IAT slot resolution order |
| ordinary `.rdata` | 96 | 92 bytes in 55 absolute pointers; 4-byte debug timestamp |
| `.xdata$x` | 330 | 311 absolute pointers to the uniformly shifted funclet band |
| `.idata$2` | 5 | Import descriptor pointer consequences |
| `.idata$3` | 0 | Exact null-descriptor contribution |
| `.idata$4` | 375 | ILT slot resolution order |
| `.idata$6` | 2,308 | Import hint/name blob order |
| `.CRT$*` in `.data` | 10 | Initializer/terminator function addresses |
| project-owned ordinary `.data` | 2,759 | 19 pointer bytes plus 2,740 compiler-private payload-order bytes |
| `LIBCMT` ordinary `.data` | 984 | 187 address-shaped bytes plus 797 archive-contribution-order bytes |
| raw-backed `.bss` | 0 | Exact 544-byte raw prefix |
| virtual-only `.bss` | n/a | Same 94,248-byte loader-zero extent |

The raw report in `build/link/HMM2PL.link.json` remains authoritative. The
classifications below explain the residuals; they do not normalize them out of
the byte count.

## Imports

Both images import the same 240 semantic DLL/name-or-ordinal identities with the
same hints and DLL descriptor order. The ILT/IAT and name blob retain a different
within-DLL order because VC6 LINK assigns slots from global undefined-symbol and
archive-resolution history. A complete WinG forcing probe moved the order but not
to retail and increased `.rdata` differences. The bounded experiment and rejection
rule are in `docs/patterns/idata-thunk-order-is-resolution-history.md`.

## Ordinary `.rdata`

Fifty-five differing dwords have retail and candidate values inside the linked
image. They account for 92 of the 96 mismatched bytes. Their eight address deltas
are exactly the already-localized function/data placement families; the reviewed
object relocations and final-image identity audit prove the same semantic targets.

The other four bytes are the `IMAGE_DEBUG_DIRECTORY.TimeDateStamp` at the start
of the ordinary `.rdata` contribution. It exactly repeats each image's COFF
`TimeDateStamp`: retail `0x3e8d400b`, while a candidate relink receives the current
wall-clock value. This is volatile LINK metadata, not a compiler datum. It remains
raw-nonexact unless the final reproducibility policy explicitly fixes both PE
timestamp fields; it must not create a `DATA_COMPGEN` source claim.

## Exception metadata

Every one of the 330 `.xdata$x` mismatched bytes belongs to one of 311 absolute
pointer dwords. All non-pointer bytes agree at the same section offsets, and every
candidate pointer is exactly 16 bytes earlier than retail. Those destinations are
inside `.text$x`: the candidate funclet band is uniformly 16 bytes early because
the preceding ordinary `.text` contribution is 16 bytes shorter. This is one
linked-placement residual, not 311 independent exception-data mismatches.

The exception structures, counts, and non-address fields therefore agree. Raw
closure depends on closing or bounding the `.text`/funclet placement frontier and
is tracked with the linked-code campaign.

## Reconstructed `.data`

Only three reconstructed owners contribute raw differences:

- `SOURCE/REQUEST`: one differing pointer byte in `cFRDummy`;
- `SOURCE/X_GLOBAL`: sixteen differing pointer bytes in scenario string tables;
- `BASE/Misc`: two pointer bytes plus 2,740 bytes in its private string-pool band.

The public allocations begin at the same section offsets and retain the recovered
types, sizes, storage, and initializers. Automatic normalized-COFF identities also
prove the compiler payload sets without source pins: base and target contain the
same 30 initialized anonymous strings for `REQUEST`, 315 for `X_GLOBAL`, and 145
for `Misc`. The only one-sided normalized names are respectively 6, 16, and 7
copies of the one-byte empty string in BSS; no initialized payload is absent.

Thus the large `Misc` byte island is compiler-private section ordering, not missing
or different literal content. This follows the Gruntz rule: content and retail
relocation/use-site evidence infer compiler-data identity automatically, while
`DATA_COMPGEN` is reserved for an ambiguous payload or a datum unreachable by the
oracles. Adding address markers to every `$SG<n>` would encode volatile compiler
state and would not reconstruct the original section order.

## CRT initialized data

The remaining 984 ordinary `.data` differences are wholly inside contributions
selected from the pinned VC6 SP5 `LIBCMT.LIB`; a further ten address bytes are in
the exact-sized `.CRT$*` initializer/terminator arrays. The candidate MAP localizes
the ordinary islands to `wincrt0`, locale, floating-point, time-zone, power-table,
and calendar members. The retail executable has neither a MAP nor an archive-member
extraction log, so its original resolution sequence is not recoverable from raw
addresses alone.

The same library archive is an authentic input, but the reconstructed project's
undefined-symbol resolution history selects and lays out its members differently.
This remains a CRT link-order wall until ordinary CRT functions and archive pulls
are semantically joined. No copied library table, synthetic root, or source padding
is retained to force its final offset.

## BSS and closure rule

The `.data` raw size, virtual size, and RVA agree. Its 544-byte raw-backed BSS
prefix is byte-exact, both images have the same following 94,248-byte virtual-only
extent, and the PE loader initializes that extent to zero. There are no raw bytes
to compare in the virtual-only span.

These findings close the reconstructed data identities and bound the linked data
sub-bands; they do not make the PE byte-exact. Raw closure still requires the
linked code/funclet addresses, import resolution history, CRT archive order, and
volatile PE timestamp either to reproduce naturally or remain explicitly walled.
