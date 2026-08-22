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
namespace cookie, which rerolled on any recompile until the second tranche gave
`gTargetName` internal linkage the ordinary way), and all 2,473 objdiff
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

## The source `VTBL()` claim channel

`VTBL(Class, addr)` and `VTBL2(Derived, Base, addr)` reconstruct the identities
`??_7Class@@6B@` and `??_7Derived@@6BBase@@@`, which
`homm2.build.annotated_vtables` and `source_symbols.collect` have always
consumed. Nothing in the source *defines* a vtable, so unlike `DATA()` the
marker stands alone at file scope in the owning `.cpp`; the `__clang__` branch
spends it on an `extern const char Class__vtbl;` declaration that allocates
nothing, purely so the `annotate` attribute has somewhere to attach and a
misplaced marker is a parse error. The compiler branch expands both to nothing,
so MSVC never sees the declaration either.

The 30 primary vtables this image emits were derived from the same donation
evidence as the data claims, with `??_7` removed from the derivation's skip
list: every one is unanimous (1 to 4 votes), none shares an address, and each
one's retail cells decode to that class's own virtual methods — `Open`/`Close`/
`Main` for the manager family, `Draw`/`??_G`/`Main` for the widget family, and
three copies of `_purecall` (0x4d9061) for the abstract `baseManager`,
`widget`, and `resource`. They occupy one `.rdata` run, 0x4ea42c..0x4eaa2c.

Codegen neutrality was measured the same way as for `DATA()`: adding the macros
and all 30 markers, together with this tranche's source corrections, left all
2,473 objdiff function rows byte-identical (`functions-exact 1727/1727`,
`matched-code-bytes 100.00%` before and after).

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

Both reviewed calls were later confirmed, and the split votes turned out to be
symptoms rather than noise: the second tranche's source corrections below made
every one of the four symbols unanimous — `gbColorMice` 14/14, its neighbour
`bLastOnscreenMouseColor` 3/3, `gArmyNames` 26/26 and `gArmyNamesPlural` 84/84.
A symbol whose votes split across two addresses is worth re-reading as "some
call site names the wrong global" before it is treated as an ambiguity.

### Second tranche: `assert_relocs` 52 → 0

The 52 rows the first tranche left behind split into three causes, and all three
are now closed. None of the work moved an objdiff row.

**38 were real defects in the reconstruction**, invisible to objdiff because it
masks relocations (see `docs/patterns/byte-identical-wrong-global.md`):

- 22 rows: `SOURCE/ARMY`'s `gTargetName`. Resolved by *linkage*, not by an audit
  exemption. VC6 decorates an unnamed-namespace object with the absolute source
  path plus a per-compilation cookie
  (`?gTargetName@?%Z:\...\ARMY.cpp1586521473@@3PADA`), so no stable name exists
  to claim and the object was the only non-deterministic input in the build. A
  file-scope `static` has exactly the same linkage and storage, mangles as the
  stable `_gTargetName`, and is what 1996 game code would have written anyway;
  the definition moved out of the unnamed namespace and carries
  `DATA(0x00524038)` (22 unanimous votes, 100 bytes of loader zero). Compiling
  `SOURCE/ARMY` twice with no source change now yields objects differing in two
  bytes of the COFF `TimeDateStamp` and nothing else, so the build has no
  remaining source of non-determinism.
- 3 rows: `bSecondAttack` (`SOURCE/ARMY`, `DATA(0x005240a0)`, 3 unanimous votes)
  and 1 row: `xRecruitmentSiteNames` (`SOURCE/KB`, `DATA(0x004ff344)`) were
  simply unclaimed.
- 4 rows: the vtables of `soundManager`, `combatManager` and `recruitUnit`
  (twice), closed by the `VTBL()` channel above.
- 6 rows: `gArmyNames` where retail reads `gArmyNamesPlural` (and one the other
  way) across TOWNMGR, GAME and SWAPMGR — a Buka localisation change, see
  `docs/version-changes.md`.
- 1 row: `CheckChangeCursor` read `gbColorMice` and `bLastOnscreenMouseColor`
  in the wrong order and used the compound `if ((a = b) != 0)` where retail
  splits the latch from its test.
- 1 row: `advManager::QuickInfo`'s two mine arms read `gResourceNames` where
  retail reads a distinct seven-entry `gMineNames` table at 0x4fe018.

**14 were delinker and compiler artifacts the audit could not express**, now
handled in `homm2.build.assert_relocs` with unit tests beside it:

- 5 rows (`army::DoAttack` ×2, `army::ProcessDeath`, `army::SpecialAttack`,
  `HandleCastSpell`, `SetMenus`): a recursive call keeps its destination inside
  the function's own carved span, so the delinker writes the final displacement
  and emits no COFF relocation, while MSVC must relocate every REL32 against the
  function symbol. `delinked_self_references` reads the delinked bytes at the
  same function-relative site and counts the call only when its displacement
  lands exactly on the function entry.
- 8 rows (`BASE/AudiereMusic`, `SOURCE/PHILAI`): MSVC names static-initializer
  thunks and their guards with a per-compilation counter, so `_$E16` is the
  sixteenth internal symbol of *that* compilation, not an identity. AudiereMusic
  numbers two ahead of retail and PHILAI's ctype guard pair is `$E18`/`$E19`
  here and `$E3`/`$E19` there, so pairing by name compares unrelated thunks.
  They are audited as one group per unit (`check_ordinal_locals`) with
  references *between* counter symbols dropped, since those cannot be resolved
  across two numberings. Only those two units differ; the other 95 name their
  thunks identically.
- 1 row (`PlayAudiereMusic`): VC6's linker folds identical COMDATs, so
  `RefPtr<SampleSource>::operator=` and `RefPtr<OutputStream>::operator=` — 81
  byte-identical instructions each, no relocations — share the single retail
  address 0xccfa0, and only one of them can be a claimed name.
  `folded_comdat_symbols` rewrites the candidate symbol onto the retail one, and
  only when the candidate object defines both COMDATs with identical bytes *and*
  retail names the other at the very same site.

Each rule was ablated against the live objects to confirm it clears its own
class and nothing else (5 / 1 / 8 rows respectively).

### The tool

`homm2 audit data-claims` is the derivation above, promoted out of scratch. It
reads the retail image, every compiled object and every file-scope `VarDecl` in
`src/`, applies exactly the rules listed here, writes `build/gen/data_claims.json`
and `build/gen/data_claims_rejected.json`, and with `--write` inserts the accepted
markers. Proposals already claimed by a marker are dropped before selection, so a
run after a landed tranche derives only what is new. `homm2 selftest audit` covers
the pure rules: the majority rule, the payload verdicts, the per-symbol vote
transposition, every rejection reason in `select_claims`, marker insertion, the PE
reader's loader-zero tail, and the extent classifier.

One caution about proving a tranche codegen-neutral from `report.json`: a
function row carries `fuzzy_match_percent`, `size` and `address` at its own top
level, *not* under a `measures` key the way a unit row does. A fingerprint that
reads `function["measures"]` therefore records `None` for every column and its
diff proves only that the set of function names is unchanged. Compare the
top-level fields, and the report's own `measures` totals alongside them.

`homm2 audit data-claims extents` audits how long a claimed allocation may be.
Two channels bound it: the retail payload (the lowest offset at which our bytes
and retail's disagree, with our own relocated fields masked) and the next
*claimed* address — a function or a `DATA()` global only, because a `const_` site
is merely an address somebody references and `&array[last]` references the
interior of the very allocation being measured.

### Third tranche: extents, file statics, and a struct that is not there

The first tranche's 78 contradicted claims are closed. 68 were arrays one element
too long — a trailing `NULL` or `0` the reconstruction invented to fill an
enum-count bound: `giPixelsPerStep` `{2,4,6,8,16,0}` against retail's five,
`gfStatPower[42]` against 41, `iMouseOffset[4]` against three,
`gResourceBaseValue[8]` against seven, and 58 more in `SOURCE/KB`. Eight were
byte arrays four bytes long (`gMapColors`, `gcGamePath`, `bStopOnTrigger`,
`gFullMapName`, and the four `REMOTE` transport buffers) and two were one byte
long (`gLastFilename`, and `ExpCampaign`'s trailing `m_pad_0x4f`).

Where a count constant bounded only proven-short arrays its value was decremented;
where it also bounds an allocation the image proves is longer, the short array got
its own bound instead. `KB_TERRAIN_TYPE_COUNT` is the instructive case: it really
is `IDX(TERRAIN_COUNT) + 1`, because `giTerrainCost` measures exactly
`10 * 4 * 2` ints, so `gTerrainNames` and `cRumourTerrainDescriptions` moved to
`IDX(TERRAIN_COUNT)` and the constant stayed.

A probe TU settles the alignment question the census raises, and the answer is
not uniform: **initialized `.data` allocations are padded up to a four-byte
boundary; `.bss` allocations are not padded at all.** `char aName[10]` is followed
at offset 12, but `char bName[351]` is followed at offset 351. So a `.bss`
neighbour pins its predecessor's length exactly (`gLastFilename` is 351 and
`ExpCampaign` 79, both unanimous), while an initialized neighbour leaves up to
three bytes of slack. Corrections in `.data` therefore take the proven bound,
which reproduces retail's layout under either reading.

**148 file statics were claimed once the vote stopped pooling them.** A static's
linker name is `_name`, unique only inside its translation unit, and the icon
decoder family really does define ten distinct `s_clipB`s. Keying the census on
the name alone merged ten units' sites into one ballot and then rejected the
result as "defined in more than one place"; keying it on `(unit, name)` — for
symbols the COFF storage class marks `IMAGE_SYM_CLASS_STATIC`, so an external
referenced from elsewhere still votes program-wide — gives each decoder its own
unanimous address. `assert_globals_data` learned the same distinction, matching
a `.cpp` definition against its own unit before falling back to the program-wide
map. The 148 are 138 decoder statics across ten `BASE/Icon*` units plus ten
ordinary ones; every address is distinct and every vote unanimous.

**`_gMidiText` is a struct retail does not have.** Its single vote placed the
symbol inside `bGotMidi`, which is what an addend computed against a *wrong*
struct layout looks like: the site is `gMidiText.filenameFormat`, retail's dword
is the format cell 0x51f594, and subtracting our `0x2c` field offset walks the
owner back into the preceding array. `"MS6b"` and `"MP1a"` — two of the eight
log-message slots that offset is made of — appear nowhere in the image, so the
Buka build compiled the MIDI logging out the way it compiled the asserts out.
The reconstruction now carries the one surviving allocation, `static char
gMidiFilenameFormat[] = "MIDI%04d.XMI"` at 0x51f594, whose padded extent runs
exactly to `crc32Table` at 0x51f5a4, and `include/BASE/MIDI_TYPES.h` is gone.

### Open queue

- **Payload contradictions: none left.** The five `COMBAT_SPELL_AI_*_MODIFIER`
  `.rdata` floats left the list because no use site negates, so the stored sign
  is the fact and the definitions are now positive and claimed at
  0x4ea80c..0x4ea81c. `szAppName`/`szTitle` left it because they are Russian in
  retail (see `docs/version-changes.md`), and with the translation restored both
  payloads match byte for byte. `gMineCharacteristics`, `gMapColors`,
  `cCombatBkgNames` and `xRecruitmentSiteNames` were length faults rather than
  content ones: their leading cells always matched and only a trailing invented
  element differed, which the extent sweep removed. `_gMidiText` was a third
  shape again — retail has no such struct at all.
- **Three symbols the derivation votes for but cannot spell as a claim.** Two
  are class statics (`AudiereMusicState::source`/`::stream`) and one is a
  `std::ctype<wchar_t>::id` guard. Each has a stable decorated name and unanimous
  votes; what is missing is a marker channel for the class-static and guard
  cases. `LoadRemote`'s `cheatWarned`/`debugWarned` and `FadeSavedUpdate`'s
  `savedUpdate` left this queue when the ordinary `DATA()` channel was applied
  directly to their block-scope static definitions; the shared Clang inventory
  and candidate binder retain their actual local-static decorated names.
- **`s_drawPixelY` and `s_drawHeroYOffset` (`SOURCE/ADVMGR`) both win
  0x523fd0.** 37 votes and 8 respectively, both with the same runner-up at
  0x523e0c, and one address cannot have two owners. The shape matches the
  lesson above — a symbol whose votes split is usually a wrong call site — so
  the likely reading is that retail has one variable where the reconstruction
  has two. `s_drawHeroYOffset` holds the claim because it was landed first;
  `s_drawPixelY` is unclaimed and the derivation reports the collision by name.
- **`_gMouseManagerStrings` stays unclaimed**, with the evidence written down:
  its 12 donation votes name **six** distinct owners — 0x11e358 (4), 0x11e318
  (4), 0x11e3b0, 0x11e364, 0x11e324, 0x135eac (1 each) — and the payload differs
  in 404 of 456 bytes. Each reference is `gMouseManagerStrings + <field
  offset>`, so a single retail allocation would make every vote agree; six
  disjoint bases is direct evidence that retail has separate `$SG` string cells
  where the reconstruction groups them into one aggregate to pin their order.
  Claiming any one address would put the symbol where five of its own
  references contradict it. `_`-prefixed internal names are outside
  `assert_relocs`'s FAKE check, so this costs no audit row.
- **`config/reloc_data_owners.tsv` recorded `gConfig` at `0x128d20`**, but 523
  unanimous votes and the donation inventory both place it at `0x1261e0`. The
  row now reads `0x1261e0`, and `reloc_owners.owners_from_rows` raises on a
  reviewed extent whose address contradicts the symbol's `DATA()` claim instead
  of silently switching the owner-extent rule off for that symbol.

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
