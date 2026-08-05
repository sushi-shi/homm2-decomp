# Reviewed public-data targets

## The source `DATA()` claim channel

`include/va.h` defines `DATA(addr)` beside `VA(addr, size)`: an absolute VA
(`RVA + 0x400000`, eight hex digits) written in front of the global's
**definition** in its owner `.cpp`. Under the compiler it expands to nothing;
under Clang it expands to an `annotate` attribute, purely so a marker written
where an attribute cannot go is a parse error instead of a silently dropped
claim. It is audit and delinker metadata, never a placement directive.

Codegen neutrality is measured, not assumed: adding the macro and then 1,018
markers left all 99 compiled objects byte-identical to the unmarked build apart
from the COFF `TimeDateStamp` (and `SOURCE/ARMY`'s per-compilation unnamed-
namespace cookie, which rerolls on any recompile), and all 2,473 objdiff
function rows unchanged.

`homm2.build.source_symbols.symbols_for_file` walks both marker families in one
parse: `VA` cursors become `kind=func` rows, and every `DATA()` marker is bound
to its `VarDecl` by `homm2.build.annotated_data.definitions_for_file` (the same
binding the data-topology and link audits already use) and emitted as a
`kind=data` row carrying the decorated linker name Clang's MS mangler produces —
`?name@@3<type>A` for external linkage, `_name` for internal. Both families
share one discipline: a marker that cannot produce a symbol raises rather than
being dropped, because the delinker would otherwise carve a span nothing can be
matched to. `collect` additionally refuses a claim whose address contradicts a
donated owner name, which would put one symbol at two addresses.

A claimed address stops being a synthetic `const_<RVA>` alias and stops needing
a donation vote; the delinked retail object names the real global, and
`assert_relocs` can resolve it.

### Evidence rules used for the first tranche

Every address came from the donation evidence transposed per symbol: in a
function whose compiled bytes are masked-identical to retail, each `DIR32` site
pairs our `(symbol, addend)` with the retail dword, so `target - addend` votes
for that symbol's linked address. A claim was written only when

- the symbol has exactly one file-scope definition, in the unit that defines it
  in COFF, and its identifier is not shared with another definition;
- the winning address has a clear majority (at least twice the runner-up and at
  least half of all votes) — balanced operand transpositions such as
  `MAP_WIDTH * MAP_HEIGHT` make the *unanimity* donation requires unreachable
  for the most-referenced globals;
- the retail payload agrees, with the object's own pointer fields masked: an
  initialized allocation must match byte for byte, a `.bss`/common allocation
  must land on zero storage, and a `.rdata` allocation must land in `.rdata`;
- no other claim wants the same address.

1,016 claims were derived that way and two more (`bLastOnscreenMouseColor`,
`gArmyNames`) resolved by review where votes tied or split: `gbColorMice` owns
`0x4f9b1c` because that cell holds its `= true` initializer, so the `= 0`
`.bss` neighbour `bLastOnscreenMouseColor` owns the loader-zero `0x536080`
between its two already-claimed neighbours. `assert_relocs` fell from 3,058
rows to 52, and 514 addresses stopped being `const_<RVA>`.

### Open queue

- **Reconstructed arrays that are one element too long.** For 70 claims the next
  claimed address proves retail's allocation is exactly four bytes shorter than
  ours (two more are one byte shorter), so the reconstruction carries a trailing
  zero/NULL element retail does not have — `giPixelsPerStep` `{2,4,6,8,16,0}`
  against retail's five entries, `gfStatPower[42]` against 41, `iMouseOffset[4]`
  against three, `gResourceBaseValue[8]` against seven, and 58 more in
  `SOURCE/KB`. These do not affect code bytes but they do move every later
  allocation in the final link.
- **Payload contradictions left unclaimed**: `szAppName`/`szTitle` (Russian in
  retail, see `docs/version-changes.md`), `_gMidiText`, `_smackMasterVolumes`,
  `gMineCharacteristics`, `gMapColors`, `cCombatBkgNames`,
  `xRecruitmentSiteNames`, and the five `COMBAT_SPELL_AI_*_MODIFIER` `.rdata`
  floats whose retail sign is positive where the reconstruction stores a
  negative.
- **`_gMouseManagerStrings`** votes for four distinct owner addresses: the
  single reconstructed aggregate stands where retail has separate allocations.
- **`SOURCE/ARMY`'s unnamed namespace.** `gTargetName` mangles as
  `?gTargetName@?%<absolute source path>ARMY.cpp<cookie>@@3PADA`, where the
  cookie changes on every compile. It cannot be claimed, it is 22 of the 52
  remaining `assert_relocs` rows, and it is the only source of non-determinism
  in the compiled objects.
- **Vtables.** Four of the remaining rows are `??_7<class>@@6B@` targets.
  `homm2.build.annotated_vtables` and `collect` already consume `VTBL`/`VTBL2`
  markers; only the `va.h` macros and the source markers are missing.
- **`config/reloc_data_owners.tsv` records `gConfig` at `0x128d20`**, but 523
  unanimous votes and the donation inventory both place it at `0x1261e0`
  (claimed there now). The stale row silently disables the only owner-extent
  rule instead of failing.

The native objdiff 3.7.1 report exposes data at section granularity. It does not
produce one report row per COFF data symbol, and no debug records carry allocation
lengths on this target (the image is stripped); the reviewed ledgers below are the
only extent authority.

Two reviewed ledgers provide exact extents. `config/required_initialized_storage.tsv`
records allocations whose initializer payload has been audited. The narrower
`config/reloc_data_owners.tsv` records public owner regions whose source `DATA()` RVA
and type-derived size are independently proven, including loader-zero `.bss` storage.
`scripts/homm2/build/reviewed_data.py` joins both to the public inventory, validates
address, compiland ownership, and PE storage, and writes
`build/gen/reviewed_delink_data.tsv`. Initialized rows additionally validate the retail
payload hash and relocation count. The generator never promotes a provisional
next-symbol gap in `build/gen/symbol_names.csv` to reviewed truth; inventory sizes
are provisional.

The patched delinker consumes the project-neutral columns `name`, `object`, `rva`,
`size`, `storage`, `alignment`, `section_offset`, `scope`, and `provenance` through
`--data-manifest`. It emits
each full definition in its explicitly named target object, preserves the manifest
`.data`/`.rdata`/`.bss` class and alignment, and converts base relocations inside the
definition to COFF relocations. Function
references to enrolled definitions become externals instead of creating duplicated
four-byte target allocations.

The header and nine-column row shape are exact. A numeric `section_offset` enrolls the
row in a candidate-topology group; `-` preserves the legacy reviewed allocation form.
`scope` is `local` or `external` and controls the emitted COFF symbol scope. Names,
object paths, and provenance
must be non-empty and contain no control bytes. Object paths are normalized relative
paths; absolute, drive-qualified, UNC, empty-component, and parent-component paths
are rejected. Names and RVAs are globally unique, extents must be non-zero,
non-overlapping, and non-overflowing, and alignment must be a non-zero power of two.
These checks are generic delinker input validation and do not depend on HoMM2 names.

The reviewed manifest is a bootstrap input, not a self-updating baseline. Normal `homm2 build` and
`homm2 status` validate and consume the existing target without deriving candidate data or invoking
the delinker. Bootstrap targets carry a bootstrap provenance stamp and remain fixed until an
explicit init.

Candidate topology is generated from source `DATA`, `DATA_COMPGEN`, `VTBL`, and `VTBL2`
annotations. The combined symbol/section manifests,
contribution replay, and exact coverage partition live under `build/gen`. An explicit
`homm2 data-topology regenerate` delinks into a temporary directory, verifies every owner object,
and replaces `build/delink` only after success. Its canonical stamp hashes all committed configs,
the retail EXE, synthetic delinker-input PDB, and delinker executable. Normal commands refuse a
stale canonical stamp and instruct the user to regenerate; they never rewrite configs or targets.
The stamp also hashes `config/delink_reloc_aliases.tsv`, whose reviewed function/address rows let
Vostok reproduce certain positive or negative COFF addends that the linked PE cannot encode.
There is no canonical unresolved-data fallback. `homm2 data-topology finalize` requires every
machine-readable symbol, section, contribution, and coverage diagnostic to reach zero.
An exact section need not have one affine retail base. When all candidate definitions are reviewed,
the section manifest retains the candidate storage/ordinal/size with `rva=-`; the delinker
zero-initializes that candidate-shaped section, copies each definition and its relocations from the
definition's own retail RVA, and rejects candidate-offset overflow or overlap. Such rows are exact
classifications in `delink_data_breakpoints.json`, not unresolved diagnostics.
Normal assembly regenerates semantic compiler-data rows from source. MSVC `$SG`/`$T`
counters are candidate topology only and never enter canonical identity. A source claim binds only
when its payload, extent, storage, placement evidence, and relocation topology select a candidate;
missing or ambiguous bindings warn normally and fail strict assembly.

`homm2 redelink` is the explicit full refresh after introducing a modeled symbol. It
refreshes function/PDB inputs, configures and builds the candidate-only `ninja base`
target before strict canonical assembly, runs Vostok once, and reconfigures the
comparison graph against the new fixed target. `homm2 init` invokes the same pipeline
and additionally prepares editor tooling. Ordinary `homm2 build` does not repeat
manifest assembly or delinking; its lightweight model census only warns about drift.

Candidate discovery is separated from that canonical path. `homm2 data-topology propose` writes
`build/gen/data_topology_review_queue.tsv`, whose non-manifest schema records individually evidenced
real placements absent from the canonical union together with proof counts and the containing open
group's blockers/contradictions. It writes no canonical or target artifact. Synthetic identities and
unevidenced rows cannot enter the queue. Promote and regenerate never read it: a reviewed row becomes
canonical only by reconstructing a source `DATA()` definition or adding an explicit
`DATA_COMPGEN()` source claim followed by canonical assembly.

The delinker manifest and parser are project-neutral. Only the HoMM2 adapter knows
about `required_initialized_storage.tsv`, so another reconstruction project
can generate the same generic manifest from its own reviewed evidence. Per-symbol
allocation scoring belongs in the generic objdiff consumer; this adapter does not
rewrite objdiff's native section measures. The final-link initialized-storage audit
remains authoritative for PE pointer-target content and final storage.

IAT slots are not project data allocations. Vostok reads their exact decorated names from the
synthetic PDB's retained inventory-backed `.idata` symbols and reconstructs `__imp__...` COFF
relocations directly; no separate IAT naming manifest is required.
