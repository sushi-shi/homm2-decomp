# Candidate COFF data topology

Candidate objects and retail evidence have different jobs. Candidate COFF proves
symbol spelling, local/external storage class, section ordinal and offset, alignment,
allocation order, and relocation topology. Retail PE bytes, reviewed relocation sites
(`config/delink_relocs.tsv` — the image has no base-relocation directory), and
reviewed owner intervals prove placement and contents; a group without reviewed
intervals stays open. Claimed inventory addresses are anchors, but do not prove
private identity, extent, TU ownership, or storage layout.

The primary mapper replays candidate sections into independent `.rdata`, `.data`, and
`.bss` streams in COFF section-table order with their encoded alignment. Public RVAs,
unique literal payloads, and relocation/addend bijections may prove individual retail
placements. Ambiguity, overlap, missing definitions, and uncovered references remain
diagnostics. Linker-sorted `.CRT$...` subsections are assigned only from an unambiguous
physical retail contribution of the same owner, domain, and size.

For a `.CRT$...` contribution, candidate COFF must contain one `DIR32`
relocation at every pointer cell. Stable target spellings bind by compiland and
name. A volatile `$E` target instead admits only same-compiland reviewed
compiler functions compatible with its complete physical extent; the counter
itself is not identity. The complete ordered pointer-choice sequence must then
occur exactly once at contiguous reviewed DIR32 sites in retail initialized
data with the candidate section's alignment. This binds the whole section or
leaves it non-affine; individual cells are never guessed. These cells are
compiler/linker topology rather than source value expressions, so they are not
given artificial `DATA_COMPGEN` occurrences.

Candidate discovery by itself is not a canonical naming source. Counter spellings such
as `$SG39045`, `$T40070`, and `name$S123`, and value-derived floating-literal spellings
such as `__real@8@3ff8a3d70a3d70a3d800`, are not source-semantic identities. A
compiler string or fixed-width `__real@` literal is nevertheless enrollable without a
source annotation when independent retail relocation evidence proves its placement.
The generated Vostok manifests combine those automatic literal rows with source
annotations and candidate COFF topology:

Function-relative relocation proofs derive their candidate extent from COFF function
type and storage class. Named internal labels, including spellings other than `$L...`,
never terminate the function census merely because they occur later in `.text`.

- `DATA(rva)` marks an ordinary named definition, including a block-scope static.
  Clang supplies its declaration name, type, and logical `sizeof`; the candidate
  binder resolves VC6's compiler-specific local-scope decoration from COFF.
- `VTBL(Class, rva)` and `VTBL2(Derived, Base, rva)` mark primary and secondary
  vtables; the tooling derives the MSVC decorated identity.
- `DATA_COMPGEN(rva, semanticName, value)` is the exceptional pin for an anonymous
  compiler-generated allocation which the automatic oracle cannot identify, or whose
  source-semantic role must be recorded. The annotation owns the retail RVA,
  semantic identity, value expression, and inferred logical size. Candidate COFF supplies
  physical topology and scope.
- `DATA_COMPGEN_GUARD(rva, semanticName, owner)` marks a compiler-emitted initialization
  guard whose value is implicit.

Disposable objdiff copies apply the same reviewed `DATA_COMPGEN` identity to the
candidate symbol only after the generated manifest's unit, section ordinal, section
offset, storage class, and scope select exactly one definition, and the reviewed
logical extent fits its physical allocation (which may include alignment padding).
This changes only the comparison symbol table: candidate payload, relocations, linked
objects, and canonical layout inputs remain untouched.

Bare string literals are the default. The automatic oracle first accepts an allocation
when equal candidate/retail function-relative DIR32 sites and the candidate relocation
addend resolve it to one retail RVA, and the complete candidate payload/storage agrees
there. This site proof disambiguates repeated text at distinct retail addresses. As a
fallback, the oracle content-matches complete NUL-terminated candidate payloads only
against reviewed retail relocation targets. It accepts that path only when the owning TU
has one candidate allocation for the payload and the payload occurs at one compatible
retail RVA. Repeated candidate allocations, repeated retail addresses, payload/storage
disagreement, and unterminated or non-padding tails are withheld rather than paired by
order. External string COMDAT copies remain independent per-object emitters and may fold
onto the one proven retail RVA.

`DATA_COMPGEN` resolves a withheld or non-string case explicitly. Its semantic name
describes the allocation's role in its TU; it must not copy an unstable COFF counter. The
generated Vostok spelling is `__h2cg$<unit>$data$<semanticName>`. One semantic identity is
allowed per TU and one compiler-generated identity per RVA. Repeated expansions of one
source macro definition coalesce; different names at one RVA are rejected. An explicit
binding reserves its candidate coordinate, so the automatic oracle cannot publish a
second identity for the same allocation.

For example:

```cpp
stream = fopen(DATA_COMPGEN(
    0x00520364, bsOpenReadStream, "rb"));

duration *= DATA_COMPGEN(
    0x004eb638, attackDurationSpeedScale, 0.08);

DATA_COMPGEN_GUARD(
    0x004ec004, sVSearchArrayInitializationGuard, SVSearchArray)
```

The annotation expands to the original value in both supported compilers and therefore
does not create a declaration. The value spelling must express the emitted allocation's
type: for example, an x87 `fmull` pool entry is written as `0.0`, not `0.0f`, even when
the eventual assignment narrows to `float`. The parser handles string literals, numeric
literals, casts, and project macros directly. It invokes the configured Clang
only for context-dependent expressions and caches per-TU results using the source,
headers, compile database, and both parser implementations. Candidate objects are
bound after parsing and are therefore not part of this source-claim cache.

The explicit binder first validates the source-inferred payload and logical extent against
retail bytes. It then uses candidate anonymous-symbol class, storage, section replay,
public anchors, and placement evidence already derived from retail/candidate relocation
analysis to find one candidate definition. Exact decorated string extents take priority.
If multiple remaining objects are physically indistinguishable, candidate stream order is
paired with retail RVA order.
The output keeps the source semantic name and retail RVA while taking section ordinal,
section offset, alignment, storage, and local/external scope from that candidate.
An exact function-relative relocation-site proof remains usable to select a
`DATA_COMPGEN` candidate even when the allocation's enclosing data contribution is still
open. This narrows only the explicit source claim; it does not close the enclosing
candidate section or waive its interval, extent, payload, or coverage diagnostics.

An external compiler-generated definition in a foldable COMDAT may have identical
candidate definitions in several objects even though the linker retains one retail
allocation. The source annotation claims that allocation once. The manifest adapter
projects the reviewed identity into every exact emitter only when the raw symbol and
the complete candidate definition/section signature agree, including selection,
association, characteristics, extent, alignment, and section checksum. Local symbols,
non-foldable sections, duplicate definitions within one object, or any signature
disagreement retain a single owner or fail validation. Vostok accepts these repeated
rows only for distinct external objects with identical RVA, size, storage, alignment,
and scope; they remain physical compiler copies, not additional semantic owners.

A normal assembly warns and omits a missing or ambiguous compiler-generated binding so
iterative work can continue. Strict assembly fails. It never guesses an identity or emits
`const_*`, `string_*`, `data_*`, `bss_*`, or another fallback spelling.

## Generated manifests and lifecycle

`homm2 data-topology assemble` writes both
`build/gen/delink_data_from_source.tsv` and
`build/gen/delink_data_manifest.tsv`. The former retains semantic source names plus
source-free compiler-literal provenance for comparison tooling; the latter is its Vostok
eight-column projection.
`build/gen/delink_common_symbols.tsv` separately carries externally linked COFF
COMMON definitions. COMMON has no section bytes or retail RVA; candidate COFF supplies
its per-object identity and allocation size, while Vostok preserves that symbol state
instead of inventing a private `.data` definition.
The pinned Vostok source is built with the repository's reviewed COMDAT-emission
patch, so the physical projection retains candidate section ordinal/offset together
with object, RVA, logical size, storage, alignment, and scope. There is no versioned
private-data supplement and no second naming ledger. The section,
breakpoint, contribution, and coverage manifests under `build/gen` retain the physical
candidate model and retail placement evidence used by Vostok.

An affine candidate section records one retail copy RVA. A fully modeled non-affine
section records `rva=-`; Vostok creates the candidate-shaped section, zero-fills gaps,
and copies each enrolled definition and relocation from its independently reviewed
retail RVA. Candidate-offset overflow, overlap, storage disagreement, and uncovered
relocation targets are hard errors. Padding is section topology, never a symbol.

The command roles are:

- `homm2 redelink` refreshes function manifests and the synthetic PDB, builds current
  candidate objects, performs strict data assembly, invokes Vostok, atomically replaces
  `build/delink`, and reconfigures the comparison graph. This is the normal explicit
  command after introducing a new modeled symbol.
- `audit` refreshes candidate objects, generated source manifests, diagnostics, and
  coverage without replacing the delinked target. `--strict` requires closure.
- `census` compares candidate and target COFF symbol/section topology as multisets and
  reports source `DATA`, explicit `DATA_COMPGEN`, and automatic compiler-string
  provenance separately.
- `propose` writes a diagnostic-only review queue. Nothing reads it as canonical input.
- `promote` refreshes generated evidence from source annotations only.
- `finalize` requires all symbol, section, contribution, and coverage diagnostics to be
  closed.
- `regenerate` rebuilds generated inputs and atomically replaces `build/delink`; it has
  no unresolved-data fallback.

Normal `homm2 build` and `homm2 status` consume the fixed target. They never derive a
candidate identity or rewrite target objects. If canonical inputs change, regeneration
must be requested explicitly.

After compilation, `homm2 build` runs a fast warning-only inventory comparison. It reads
candidate COFF and existing generated manifests but does not inspect retail placement,
invoke Clang, assemble canonical inputs, or run Vostok. New or missing function/data
definitions and fallback target identities print a `homm2 redelink` reminder without
failing the build; strict modeling remains the responsibility of explicit redelinking.
An MSVC `$S<number>` suffix on an ordinary source-named static is accepted because its
`DATA` row supplies the semantic identity and exact candidate binding. Anonymous `$SG`,
`$T`, `$E`, and fallback target identities are not accepted.

During reconstruction, annotations stay next to the expression or definition whose
semantics they record. This supports half-built TUs because source supplies the semantic
claim while missing candidate evidence is only a normal-mode warning. After the entire
project is matched, a cleanup branch may mechanically extract all `VA`, `VA_COMPGEN`,
`DATA`, `DATA_COMPGEN`, `VTBL`, and related annotations into generated tables. That is a
representation-only finalization step, not a second source of truth during decompilation.

The permissive Vostok option `--recover-data-relocs-from-pdb` remains bootstrap-only. It
may create synthetic nearest-symbol identities and is never canonical ownership evidence.
