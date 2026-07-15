# Candidate COFF data topology

`python3 -m homm2.build.candidate_data_manifest` inventories definitions directly from each
compiled candidate object. Candidate COFF proves symbol names, local/external scope, storage
class, section-relative topology, alignment, and relocation spelling. Retail PE bytes and NB09
public symbol RVAs and `sstModule` contribution ranges remain authoritative for addresses and
contents. The shipping NB09 stream has no game procedure, local, type, or line records; this
process does not infer any of those from CodeView.

For a function, candidate `DIR32` relocations are paired in order with retail HIGHLOW sites only
when their counts agree. Known public-data relocations anchor the pairing. Every occurrence of a
candidate-defined symbol must then identify one retail RVA, every definition must map to a
different allocation, and every mapped extent must remain within the same TU/storage contribution.
Before a group closes, every retail HIGHLOW target in that contribution must be owned by one of its
allocations. Ambiguity, overlap, a missing definition, or an uncovered reference leaves the whole
group open; the generator never emits a partial closed-world group.

The generated reviewed manifest records candidate section offsets and symbol scope. The pinned
delinker emits closed groups in that topology, resolves interior references as owner plus addend,
and errors if a relocation reaches an uncovered address in a closed group. Undefined candidate
externs remain undefined; retail storage is never copied into the referring object.

The compatibility option `--recover-data-relocs-from-pdb` retains permissive nearest-PDB-symbol
recovery. It now preserves read-only `.rdata`, initialized `.data`, and zero-fill `.bss` storage;
it can still produce synthetic `const_` or string identities and is not proof for canonical data
ownership.

`--require-all` turns the inventory command into a canonical-closure gate: it writes no manifest
and exits unsuccessfully if any candidate data-bearing group remains open. Without that flag the
tool emits only independently closed groups so they can be reviewed and integrated incrementally;
that partial output is not evidence that the program-wide topology is complete.

## Current census

At the implementation checkpoint, candidate objects contain 6,733 defined symbols across 163
data-bearing object/storage groups. This denominator includes public data, compiler-local statics,
string literals, constant pools, vtables, and other compiler-emitted definitions, not just the 281
source lines spelling `DATA(...) static`.

The first fail-closed pass proves 226 definitions in 59 complete groups. The remaining 104 groups
are reported explicitly: 58 initialized-data, 29 BSS, and 17 read-only-data groups. Diagnostic
classes overlap because one group can fail several checks: 85 have a referenced retail RVA not yet
covered, 70 contain an unmapped candidate definition, 18 have an ambiguous relocation mapping, 14
use multiple candidate COFF sections, 24 produce overlapping inferred retail extents, 10 have an
extent outside the contribution, and 9 fail the one-to-one mapping check.

`BASE/Icondf2b` is the regression fixture. Its 37 candidate/retail `DIR32` sites, including two
public `uDimPal` anchors, prove a bijection for all thirteen four-byte function statics. The target
now emits their exact `_gFD*$S...` names and candidate offsets in a 52-byte BSS section, replacing
the former `const_001381xx` and unrelated string identities. Its focused score changes from
80.283134% to 81.06626% under `data_value` and to 81.03615% under `name_address`; the ordinary code
score remains 81.1265%.
