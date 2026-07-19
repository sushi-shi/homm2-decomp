# Matcher experiment matrices

> Historical evidence only. The retained-maximum baseline and its update/check
> commands have been removed; live bytes, relocations, and the current report are
> now authoritative. References below describe the recorded campaign state and
> are not instructions for the current workflow.

These TSV files are durable no-repeat sets for large exact-preserving searches. The original
matrices contain:

```text
whole-source-file SHA-256<TAB>focused function match percentage
```

Newer matrices add a third `outcome` column. Always follow the file's header; the outcome records
whether all pins held or names the raw pin/review that rejected the state.

Before replaying a source state, compare its hash here and confirm the canonical source and pinned
sibling hashes have not changed. A listed hash should not be rebuilt under the same compiler/header
state.

## BASE/icon2bc clean reconstruction

`icon2bc-clean-reconstruction-04f798c.tsv` preserves the complete stale-header clean-room search,
whose best state was 62.85%. `icon2bc-clean-reconstruction-a45e64f.tsv` records the fresh search
after rebasing to checkpoint `a45e64f` and the current `icon::m_data` type. The canonical body was
not inspected or copied. Its locally measured score was 72.22% (retained campaign maximum 72.479%);
the best independent decoder reached 69.46%, 90/91 relocations, and was rejected. Canonical source
was therefore restored. Its full-range raw audit confirms a structural residual: after masking the
union of relocation payloads, 745 of 792 common bytes differ and retail has a further 0x49-byte
tail.

The new search recovered reusable structure: the destination is a long-lived register cursor with
selective scratch publications; the dim path republishes clipped counts and palette state; and the
literal path has a common zero-count publication join, including the fully clipped case. Parsing
`Misc.h` reproduces the retail private-symbol sequence `S1629` through `S1645`, which is strong
evidence for the historical include/TU state. A direct-global decoder was again rejected because it
overpublishes scratch state and loses the retail register lifetimes.

The best clean state still has a global register-role swap (candidate X in EBX and destination in
ESI versus retail X in ESI and later destination in EBX). Its relocation residual is missing two
`gCTClipR`, one `gCTCnt`, and one `gCTSrc` reference while adding one `gCTX0` and two `gCTY`
references. Do not repeat local predicate/count synonyms. The next credible lever is an
exact-preserving predecessor/include/TU-state and lifetime search under the combined translation
unit, retaining canonical unless the 72.479 maximum is exceeded.

No permutation tool was used in either search. The regex permuter remains prohibited; AST
permutation is inappropriate until the body is structurally aligned at the 96-97% last-mile
threshold. The old/new matrix SHA-256 values are respectively
`2634ba3204bd39929fd97602e87484522a145105f93e353179264461c6b603eb` and
`7cb7a33ae738a510c08cbc6a23c4a2c3b8201f469df34c95aab0cf601b60fa8f`.

## BASE/Misc early-batch structural and TU-state retest

`misc-early-tu-state-04f798c.tsv` records the bounded clean retest of `InitMemEntry` through
`FindToken` from checkpoint `04f798c`. Its hashes identify the normalized descriptor printed in
the `variant` column (SHA-256 of `scope:variant`), rather than unavailable temporary whole-file
states; this makes every rejected spelling independently searchable and reproducible.
The checkpoint `Misc.cpp` SHA-256 was
`08b3d2dfb8a14899ad3cac10418aa1ad82124cfcc0195eeea45e310efcf09194`; the matrix SHA-256 is
`c7ca8e27b1047bd4f7e5692a6aa906ec06be6df86f3857d6e48ad90700342617`. Later-function
reconstruction changes the whole-file hash without changing these predecessor descriptors.

The important structural recovery is `FadeIn`: naming the repeated `0x3f - level` value as an
explicit `int threshold` changes MSVC 4.2's allocation and reassociation to retail, raising the
function from 92.96% to 97.41%. Candidate and retail are both 0xea bytes, all 11 relocation targets
agree, and all bytes after +0x1f align. The remaining raw difference is only the six-byte new-
expression null-check ordering at +0x1a..+0x1f; three local allocation spellings leave it unchanged,
so it remains an `@match-note`, not a proven wall.

The BaseAlloc append spellings also demonstrate why a local `/O2` search cannot certify a wall:
changing only the append and a value-neutral size alias moves later functions substantially, making
`FindLastToken` exact in several rejected states while regressing BaseAlloc and other pins. The
canonical high-scoring append was restored. Future work should start with exact-preserving
predecessor/TU-state variants and the recorded pins, not replay these local synonyms. No regex
permuter was used; the listed relational and SIB forms were manually audited as value-preserving.

## BASE/Misc game-default, registry, and CD structural pass

`misc-game-registry-cd-a45e64f.tsv` records the source-order pass from `SetGameDefaults` through
`SetupCDDrive` on checkpoint `a45e64f`. Descriptor hashes are SHA-256 of the exact
`scope:variant` strings, so rejected temporary spellings remain searchable even though their
whole-file states were not retained. The retained `Misc.cpp` SHA-256 is
`38f67aa8d1b6e870596d67d5a2cb1322c1100b1f17ee8933422d6cf9302b29b0`; the matrix SHA-256 is
`4ef70591756b09513cf0aae0109b0843c9b8f7bcdf28eb76d8622a8e81780d2c`.

The two registry functions were not relocation-only walls. Raw prologues exposed undersized
`szScratch[88]` locals: base reserved `0xc8`/`0xc0` bytes while retail reserved `0xd4`/`0xcc`.
Recovering the real 100-byte buffers makes `ReadPrefsFromRegistry` and
`WritePrefsToRegistry` exact. Their relocation counts are respectively 126/126 and 90/90; the
read helper displays most `gConfig` member addresses under different delinked retail owners. A
later addend audit found one genuine exception at function `+0x2f6`: retail stores to
`gConfig.uniqueSystemID[3]` (`gConfig+0x125`), while the former source incorrectly stored to
`gConfig.modemInitString[98]` (`gConfig+0x110`). Correcting the field preserves all code bytes and
makes all 126 relocation targets agree.

`SetupCDDrive` similarly disproved its old raw-identical claim. Widening the real local lifetimes
first recovered the `0x2f0` frame and raised the live score from 99.47% to 99.94%. Declaration
order and local-name spelling did not affect optimized allocation. The decisive source shape was
an explicit retry-loop scope around the 256-byte character result buffer and 256-byte command
buffer. That permits VC4.2 to reuse the finished drive-count slot and places the buffers at retail
offsets `+0x38`, `+0x138`, `+0x19c`, and `+0x29c`. All `0x3ed` code bytes and 51/51 relocation
targets are exact; the three following retail NOPs are alignment outside the function symbol.

`SetGameDefaults` remains a structural `@match-note`, not a certified wall. Its source writes the
same two graphics records and has the same CFG/calls, but base induces from the record start while
retail induces from `fullScreen`. The matrix lists every rejected local loop/anchor spelling.
Resume only from a new exact-preserving predecessor or shared-header TU state; do not replay the
local variants. Neither permuter was used: the function is only 90.21% and is not structurally
aligned enough for the audited AST last-mile tool, while the regex permuter remains prohibited.

## BASE/Misc terminal structural pass

`misc-terminal-345e643.tsv` records the bounded source-order pass from
`BlitBitmapToScreen` through `DataEntryWindowHandler` at checkpoint `345e643`. Descriptor hashes
are SHA-256 of the exact `scope:variant` strings. The retained `Misc.cpp` SHA-256 is
`d920abfe77a12b518442adcf0d4a802a91231c535ff9342864fcca214ab6e9e6`; the matrix SHA-256 is
`305297a9401ca13bf62d9def7f39e80295491b899a6bc0d480612e7a3cf170a5`.

`BlitBitmapToScreen` was not an optimizer wall. The Ghidra-shaped second overlap predicate used
four AND terms with inverted comparisons, while retail branches on the OR of four disjointness
tests. Recovering that predicate and the entry bitmap spill restores the 8-byte frame, the full
four-VESA-call CFG, and equal 0x18c object extents; the score rises from 80.04% to 95.53%.
`GetDataEntry` likewise needed the real conditional Y adjustment rather than its boolean mask.
That makes both code spans 0x386 with an exact 0x9c frame and leaves only 12 unmasked bytes at one
constant-load schedule and two local LEAs. Moving `DataEntryWindowHandler`'s possible-cancel body
before the normal fallback recovers retail body order and raises it from 86.76% to 93.80%.

The matrix also pins the rejected combined-TU retests: `strcat` regresses all three logging
functions, a volatile `FadeTo` level regresses its frame/schedule, the two PCX polarity/SIB forms
emit no useful gain, and a volatile `SGenRand` store regresses the entire random cluster. Direct
handler returns fall to 48.09%; a volatile tail field read emits no focused change. None of these
families should be repeated while the retained source/header state agrees. No permutation tool was
used in this pass. In particular, the regex permuter remains prohibited; if the 99.10%
`GetDataEntry` residual is revisited after an exact-preserving predecessor/TU-state change, only
the audited libclang AST permuter is eligible.

## BASE/Icon2b adjacent-decoder setup transfer

`icon2b-adjacent-setup-dd7973b.tsv` records the bounded setup-lifetime retest from checkpoint
`dd7973b`. The Icon2b source and its complete include surface were unchanged from the documented
`4498053` state, so this was not a new compiler/header TU state. The only retest trigger was the
later reconstruction of the adjacent `Iconf2b` decoder, which supplied retail-evidenced early
source-cursor and entry-Y lifetimes.

Three entry/source-publication variants regressed the score and shortened the candidate from
0x4c7 to 0x4c5-0x4c6. A separate `entryY` field snapshot produced the exact same emitted `.text`
SHA-256 as canonical. The canonical source was restored. No dim/literal axis from the soft-defer
ledger was repeated, and neither permutation tool was used.

`icon2b-clean-room-a45e64f.tsv` records the later clean-room restart and is the authoritative
no-repeat ledger for that pass. Its SHA-256 is
`4f75bd1b9ff72f8b62b6551d12bf924101a3dd4917c46863cfa85aab1362c51d`. A `not-captured` text
hash means the transient `.text` artifact was not retained; the full source hash plus score, size,
relocation count, tested axis, and outcome still identify that state. The canonical implementation
was mechanically hidden before any candidate work and its body was never inspected or copied.
Reconstruction used only the public ABI,
retail disassembly and relocation stream, CodeView layouts, cached decompiler semantics, adjacent
decoder conventions, and the canonical file's declaration/include surface. The latter was consulted
only after an independent candidate had been compiled and measured.

The pass crossed two base identities. The first candidate was made at `38b2438`; after the lane was
reset, all later retained measurements used `a45e64f`. The canonical `Icon2b.cpp` source hash was
`b42c0b8e368561082811dbcc7b170ab975483447470c577173ad6a9132f26124` at both checkpoints. At
`38b2438`, the relevant input hashes were `icon.h`
`84030c0e0ea1296a34cfd9cb19f450af59326522c7d983d78485eec09d97aaf0`, `IconEntry.h`
`4c213fcda960752692e6f1e202231b26d523bc2fa259c0b8a2d573380d2fb805`, `Icon2b.h`
`64e20fdcad765b0cf4a2f3aa1c39c424061dc1e2c9211471da8ed6df5f947530`, `bitmap.h`
`2bdd293d52be21a46247a9257c15ebe2503cf38e50cbc7b3488c82a72127727f`, `X_GLOBAL.h`
`2a7fce66bcff3f06b7186bb8477d338bdaa42febc708ae104bb22d424ee015e0`, and the baseline
`9944ebd7d571e578215c686faaead49b869af9fe7b1cb1f7f1588c3632b0cda6`. At `a45e64f`, the
relevant inputs were:

- `include/BASE/icon.h`:
  `809577ac8c7a091878b5ff40de5d96d37fe2074f480d45207ba6d7ccd3ef4e22`;
- `include/BASE/IconEntry.h`:
  `4c213fcda960752692e6f1e202231b26d523bc2fa259c0b8a2d573380d2fb805`;
- `include/BASE/Icon2b.h`:
  `64e20fdcad765b0cf4a2f3aa1c39c424061dc1e2c9211471da8ed6df5f947530`;
- `include/BASE/bitmap.h`:
  `2bdd293d52be21a46247a9257c15ebe2503cf38e50cbc7b3488c82a72127727f`;
- `include/SOURCE/X_GLOBAL.h`:
  `bf73ee8242bf9ef85786075a62fea0a4501faea2d71fde8068d9b863aa6d8914`;
- `config/match_baseline.tsv`:
  `7dabd2551254fa709b797049e36eb20a5ea355430e5af159c72619a15b464aa8`.

The first independent body accidentally shifted the integer roles instead of using the retail map
`x, y, frame, clip, clipX, clipY, clipW, clipH, color`. It scored `54.535713%`, emitted `0x4a3`
bytes with an unwanted `0x10` frame, and had 79 candidate versus 83 retail relocations. The matrix
retains its source and `.text` hashes so this invalid ABI interpretation cannot be rediscovered and
retried.

The remaining 20 rows record every measured structural step, including regressions and emitted-code
no-ops. The productive sequence was: correct the parameter roles; recover the retail three-call
clipped fill; repair `gIcCnt2` publication; use one literal quadrant selector with one common
`memcpy` tail; place the pitch lifetime before clip classification; split entry X from the running
X; repair dim palette/destination/count lifetimes; and restore the exact include/scratch declaration
surface with a typed `IconEntry` local. The independent best was
`ec6beeb17c9db42ba48d6cbd3fea9ed4a0d8bc4c4500eeba3cdf7dd1670db9f5` at `73.563190%`, size
`0x4e9`, with all 83 retail relocation records and `.text` SHA-256
`b134d637dfe087a246840e3b5bdcf6763940b605694dbc059e678c08d9a8bdd9`. An unsigned-byte entry
cursor regressed to `72.079670%`; splitting the `gIcClipR` snapshot declaration from its assignment
was byte-identical to the independent best. These two axes must not be retried in the same TU/header
state.

The independent best did not clear the retained canonical maximum of `74.453300%`, size `0x4c7`,
79/83 relocations, `.text` SHA-256
`d5e105683f769459d8f3399fdac4f4a15faa54516b74070090d98ed7c34bb4cb`. Therefore no candidate
was integrated: the canonical source was restored mechanically without viewing its body. Its live
score after the `a45e64f` combined-TU/header changes is lower than that retained source-hash maximum;
this is expected cumulative compiler state, not evidence that the clean-room candidate should replace
it. Future work should begin from a genuinely changed shared-header/predecessor state and the concrete
setup register-allocation or fifth dim-publication residual, not replay any matrix row. Neither the
AST nor regex permuter was used; AST permutation remains inappropriate until the function is truly
structurally aligned at 96-97%, and the regex permuter is prohibited for this campaign.

## BASE/SAMPLE whole-TU audit and constructor shapes

`sample-tu-audit-66dafcc.tsv` is the retail-order audit of all eight CodeView functions at
checkpoint `66dafcc`; its SHA-256 is
`00cfc7633163c4e0d766280db792baffb61319ca48c4083d984d1d571d5ea6c7`. The audit reads COFF
section bytes and relocation tables directly because `homm2 relocs` stops at the constructor's
delinked local jump-table identities and cannot associate a candidate `??_G` deleting destructor
with the duplicate retail `??_E` symbols.

The standalone `sample` destructor, `MIDIWrap` constructor, and `MIDIWrap` destructor remain 100%
raw exact at 4/4, 12/12, and 4/4 relocations. Each generated deleting-destructor candidate is also
raw exact against both of its independently delinked retail copies: `sample` is `0x41`, 5/5
relocations, and `MIDIWrap` is `0x3d`, 5/5. These four unscored CodeView aliases are strict
delinker/symbol-identity artifacts, not incomplete functions.

`sample::sample` is semantically and structurally complete but remains an unresolved compiler-shape
residual, so its source marker is `@match-note`, not `@early-stop`. Candidate and retail are both
FPO, size `0x181`, with the exact `0x20` filename area, callee-save set, stack accesses, CFG, switch
tables, load tail, and 23/23 relocations. Relocation-masked raw comparison differs in 27 scheduling
bytes at `+0x24..+0x25`, `+0x29..+0x39`, `+0x43..+0x48`, and `+0x4b..+0x4c`; every byte from
`+0x4d` through `+0x180` is exact. The base hoists `EBP=2` across `resource::resource` and places
the inline-`strcpy` EAX zero after the volume store, while retail schedules them after the argument
reloads and before that store.

`sample-ctor-shapes-66dafcc.tsv` records the bounded no-repeat source-shape pass; its SHA-256 is
`fe1b3861b7a012915e9e9b274b6c498fbe0af3e4f308c46660f6252021d44f15`. Joining the declaration
and initialization before the member stores is byte-identical to the retained split form. Moving
the initialization after `m_channelType`, with either joined or split declaration, emits the same
regressed `96.47%` body and perturbs argument reload plus inline-`strcpy` register selection. All
three exact authored pins remain exact in every state. Do not repeat these shapes under the same
source/header/TU state; revisit only after a real state change or in the >=95% last-mile phase.
Neither permutation tool was used.

The two recovered numeric domains in `include/BASE/sample.h` were also normalized from plain enums
to header-level `typedef enum` declarations without changing any value or storage. The header moved
from SHA-256 `a556d90f3c4a268140249b2b72eceb6643fdf826628d0c4abdf87c3a9a150298` to
`15ec2254596e7d8f9773b842193f5c2ba83b6ad6072fa948ef231b00122d3648`. Rebuilding SAMPLE and all
five dependent TUs left every SAMPLE `.text` hash in the audit unchanged.

## BASE allocation/assert metadata wrapper sweep

`base-misc-callsite-audit-8ed859f.tsv` records all 86 direct `BaseAlloc`, `BaseFree`, and
`ProcessAssert` callsites converted in the non-sound-manager BASE scope at checkpoint `8ed859f`.
Each function row preserves the call order, old metadata form, retail path/line recovered from its
RVA, pre/post score, relocation result, and complete pre/post TU hashes. The three implementations
in `Misc.cpp` were intentionally not wrapped. No permutation tool was used. The callsite matrix
SHA-256 is `b8d87564501ab3c512e2fad74320dee80e8b3a96b29656936ada468e161b2c17`.

The matrix abbreviates only the uniform retail directory prefix
`I:\Projects\Heroes\Prog\BASE\`; every basename retains its retail case. The wrapper header moved
from SHA-256 `2719f98ca758614124a7138e6ee870e1ef6f9d9ccf845bcb88e39ae5e427131a` to
`5f54db2d7e6dd85cb245c8cab8aa0b24d79aec998ab3bbf558dc24c594433233`.

`base-misc-wrapper-tu-state-8ed859f.tsv` is the no-repeat matrix for the wrapper and shared-header
states. Parenthesized and token-preserving function-like wrappers emitted the same live code, but
the new direct-function hashes exposed retained-max dips in `DoBlur` and `CreatePCXFile`. The actual
lever was `IconEntry`'s declaration source location: parsing it at its historical position in
`Misc.h` restores 93.9030% and 98.0405%. Doing that globally either duplicates the type or loses one
net exact function by regressing the exact `fullGt` and `ProcessMouseMessage` pins while gaining an
unrelated miscwin exact. The retained form opts only Blur and Misc into
the inline declaration while every other TU keeps the existing `IconEntry.h` include state. Do not
retry the four rejected full header/source hashes while the retained hashes agree. The TU-state
matrix SHA-256 is `013d8d3c3e2e62712d95ff4a157e1b34342cfc733066c49b9f9aa5213df1a365`.

The first build also rejected three listbox sites because their existing `#line` directives
occurred between arguments; preprocessing directives cannot occur inside a macro invocation.
Moving each directive immediately before its statement compiled and preserved the exact destructor
and `DeleteItem` pins plus the 99.61% `Main` score. Do not put `#line` back inside an `H2_*`
invocation.

Retail metadata made all three PALETTE functions, both TILESET functions, and
`resourceManager::Close` newly exact. `mouseManager::Close` and
`resourceManager::LoadAggregateHeader` improved slightly. Every old exact pin stayed exact, and all
other focused scores stayed unchanged or recovered their retained maximum. The baseline was updated
with plain `homm2 status update`; `--accept-regressions` is not valid for these direct edits. The
relocation helper's three MOUSEMGR aggregate/member
alias warnings and the SAMPLE/Textntry/dropList/listBox delink-boundary undercounts predate this
sweep; they are recorded rather than misclassified as wrapper regressions.

## BASE/soundmgr whole-TU raw audit

`soundmgr-raw-audit-72ca327.tsv` records the complete 34-function audit from checkpoint
`72ca327`. No source-shape experiment and neither permutation tool was run in this wind-down pass.
The canonical pre-comment source SHA-256 was
`6ea419d2e3b079228fdc16d57e27eddcee84d82d3d73ddaa0e8897fadc50b761`; the unchanged
`include/BASE/soundmgr.h` SHA-256 was
`e6bf228b4dc73f028aca22ba12986c67993108878c146fdcdac96f7471a9b48b`.

The audit extracts each complete function from the compiled and retail `.text` sections, masks
the union of four-byte relocation payloads reported by both objects, and compares every remaining
byte. All 27 objdiff-exact functions are raw-exact. `ModifySample` is also raw-exact over its full
0x202 bytes after masking 23 relocation offsets, including its address table at +0x16e..+0x182,
byte-index table at +0x182..+0x1e7, and final code at +0x1e7..+0x202. Its sub-100 objdiff score is
therefore a permanent fixed-IAT/delinked-self-relocation identity wall.

The other six residuals are not relocation-only walls. The TSV records every unmasked byte:
`CDStartup`, `CDPoll`, and `PlayAmbientMusic` retain one local-branch displacement each;
`CDSetVolume` swaps adjacent 0x0b/0x0c immediates; `StopAllSamples` retains ten stack-slot
displacements; and `SwitchAmbientMusic` retains two branch displacements and one immediate. Their
old `@early-stop` claims were removed. Do not treat `homm2 sema disasm --diff --lite` showing only
relocation-name differences as raw proof: lite normalization also hides branch destinations,
stack displacements, and immediates. Re-run the full relocation-union raw comparator after any
future predecessor, header, or combined-TU state change.

`soundmgr-structural-fixes-72ca327.tsv` records the subsequent authorized six-function repair.
No permutation tool was used. `CDSetVolume` needed the retail arithmetic
`0xc - (0xb - volume / 0xc)`, correcting both emitted constants and the decomp's semantics.
`CDPoll` needed one combined `CDPlaying == 0 || m_currentTrack < 0` guard.
`StopAllSamples` used `od_slots.py` to select the semantic names `sampleIdx`, `waitCounter`, and
`sampleStatus`, whose buckets 1/2/3 reproduce retail's -0x4/-0x8/-0xc slots.

The first explicit-return state retained the surrounding `else` chains. It made the arithmetic,
combined guard, and slots exact but added continuation jumps to `CDStartup`, `PlayAmbientMusic`,
and `SwitchAmbientMusic`; hash `4b618e87...` must not be retried. Removing those trailing `else`
chains recovered retail's actual guard-clause CFG. `SwitchAmbientMusic` additionally required the
retail-proven `m_fadeSteps <= 0xa` threshold, rather than the decomp's incorrect `<= 0xb`.

The retained source SHA-256 is
`9f170036461a1a79ab2a7894f0402ad271573cf647bee0577c4a56f4fd34e487`. It produces 33/34 objdiff
exact functions; the remaining `ModifySample` row is full-function raw-exact after relocation-union
masking. Thus all 34 functions are raw-exact, with only `ModifySample`'s permitted delinker identity
wall below 100% objdiff. All 27 pre-existing exact siblings remained pinned throughout.

The subsequent assertion-wrapper recovery at source-decomp checkpoint `226828f` replaced the three
remaining direct `ProcessAssert` calls in `ValidatePreviousPosition`, `ModifySample`, and `PollSound`
with `H2_ASSERT`. `homm2 sema strings` at retail RVAs `0x0cb6a0`, `0x0ccc80`, and `0x0cd320`
independently recovered the same original filename, `I:\Projects\Heroes\Prog\BASE\soundmgr.cpp`;
the existing retail line arguments `66`, `0x52f`, and `0x61a` and each assertion expression's token
shape were preserved. No source-shape experiment or permutation tool was used. The source hash moved
from `9f170036461a1a79ab2a7894f0402ad271573cf647bee0577c4a56f4fd34e487` to
`cc03a7df2e5b3aae3e11ba5c78cd538148a0baf27802e8494d987e843559244a`, and the candidate object hash
from `d3821307056a0da061096dc9efd6e8e457be27e99704ed2076fb962f5c034a52` to
`48768cc90911be6b39ee041731bddd0542fdb610773d4c0faab4e755b6606e78`. A fresh complete-function
COFF audit again found all 34 functions raw-exact after masking the union of four-byte relocation
payloads. Objdiff remained 33/34 exact, with only `ModifySample` at `99.837960%`; its full `0x202`
span remained raw-exact with 23 candidate and 20 retail relocations. Do not retry filename casing,
`__FILE__`, expression synonyms, or either permuter for these wrappers.

## BASE/Textntry AST searches

All three searches used only the then-current `scripts/permute_ast.py`, never the regex permuter,
and required all 11
siblings to remain pinned:

- `textntry-ctor-ast-e4e96c0.tsv`: initial constructor pass, 562 unique hashes; SHA-256
  `150e0e735e04b738cb4cf3fe17cf2308e3a5f2ff878f898a65f4b60f65449e75`;
- `textntry-read-ast-ctor8aaf3.tsv`: `Read` pass from the improved constructor state, 356 unique
  hashes; SHA-256 `de0d4815b5c32b62e679891af36c71bb7865589814ea63dd112527786a705b0c`;
- `textntry-ctor-ast-8aaf3-deep.tsv`: constructor continuation, 819 unique hashes; SHA-256
  `0e5a6584c1c4786abf35867a9c1072070ced7586d13f64bd69ac67d0354c9f8a`.

The initial constructor pass found the retained 98.695656% source state. The `Read` pass and deep
constructor continuation found no further improvement.

## BASE/Textntry include-surface audit

`textntry-include-surface-8aaf3.tsv` (SHA-256
`d58c40bf716815c90a362b6f0f5fbd6a733105499dd25ea089ee57ad11e13f0a`) records the canonical state
plus eight previously untested declaration/include surfaces. The matrix includes every objdiff
function row and retail size; the two deleting-destructor aliases remain unscored because the
canonical standalone-destructor tradeoff emits one 0x1f base wrapper for the two retail 0x36
aliases. All scored siblings and sizes remained pinned. Canonical and the redundant-`va.h` removal
also produced the same aggregate emitted `.text` dump SHA-256,
`d68541c49e4bf533e079cc8410cc8d24d82b6ad72d3d3010be1fd16c74238d4f`.

The audit established that `BASE/Misc.h` is still required as the owner of `BaseAlloc`/`BaseFree`,
`SOURCE/KB.h` is still required as the owner of the manager globals, `glTimers`, and
`gbTextEntryEscaped`, and `SOURCE/X_GLOBAL.h` and `SOURCE/kbwin.h` own the remaining globals and
functions. Only the direct `va.h` include is redundant, and removing it is byte-neutral, so the
canonical include block is retained rather than creating a no-gain source hash.

## BASE/Textntry fresh SetupDisplayString structure pass

`textntry-setup-fresh-2e351c8.tsv` records the independent retail-structure pass from checkpoint
`2e351c8`. The retained second-loop guard and explicit zero-offset clear raised
`SetupDisplayString` from 95.000000% to 97.128380%, matched the retail `0x1be` size, and kept the
`0x130` frame and 8/8 relocations. The remaining code difference is the whole-function EBX/EBP
allocation swap between `this` and the cursor/second-loop flag lifetime.

The final whole-file source SHA-256 is
`fd22c7692d4e9293c47a13eb83b422b678371fe4ee19755d78b515c9b94fd5cb`; the unchanged
`include/BASE/textEntryWidget.h` SHA-256 is
`f5d08f5a98e96310182cf720733ccb4670b41bfcb24c44ec8ce36fcce86072a3`.

Every row pins the same sibling baseline: default constructor 100%, long constructor 98.695656%,
destructor 100%, `Read` 98.675500%, `Main` 99.852940%, and `Draw` 99.992424%; deleting-destructor
aliases remain unscored. The cursor snapshot, `register` snapshot, and loop-flag declaration-order
states are byte-identical at the new source structure and must not be repeated while the final
source/header hashes agree. No permutation tool was used.

## BASE/Textntry SetupDisplayString AST pass

`textntry-setup-ast-341e775.tsv` records a focused pass with the then-current
`scripts/permute_ast.py` from integrated
checkpoint `341e775` and source SHA-256
`fd22c7692d4e9293c47a13eb83b422b678371fe4ee19755d78b515c9b94fd5cb`. The pass exposed 18 legal
first-order variants and ran a deterministic 300-attempt walk with seed `341775`. Existing matrix
hashes were skipped before compilation, and unsafe inequality +/-1 rewrites were disabled.

The pass compiled 286 new unique whole-file hashes. Every candidate remained at 97.128380% and
kept all 11 objdiff symbols pinned, so the integrated source was restored unchanged. The matrix
SHA-256 is `ec3473c9c38668f263892a0a5868cd8280ec34c01d28c9359b95064be7ec8344`. Do not replay these
hashes while the source and header state agree. The regex permuter was never used.

## BASE/iconf2by manual reconstruction

`iconf2by-manual.tsv` contains 136 full-SHA manual source states spanning the 84.29% through
93.72654% reconstruction. Each row records the source hash, source-shape label, match, candidate
size/frame, relocation counts, and disposition. Its SHA-256 is
`f787d7905c05b7a28435c8bd9e6296f2075c7c4579f4cd744874c57330fdac5d`.

The current source is restored to
`f33ab84fe5e3b9205e40ae09de314bced82063fe587bddee30646c73d44bfea0`. The first normalized
register divergence is in the prologue: candidate `+0x5c` loads `shear` into EBP, while retail
loads it into ESI and then loads `clipW` into EBP at `+0x60`. The later missing `gFYClipR` reload is
now recovered by the retained first-use scratch declaration order with `gFYClipR` last. That order
raises the match to 93.72654%; the branch-local fill snapshot keeps the fill `gFYRun` load after
the vertical checks, matching retail's source lifetime. CodeView has no symbol
subsection for this TU, so neither stack-local nor optimized-register names exist; the matrix—not
guessed local names—is the no-repeat oracle. No permutation tool was used.

The integrated-root `4d381e1` continuation added 18 combined-TU states. Direct, commuted,
`register`, and top-level-`const` width/shear/fill spellings were byte-neutral; retail-order edge
publication remained 92.544235% and did not free EBP; restoring the owner/Misc declaration surface
fell to 84.28954% with 141/144 relocations; and an unsigned global run fell to 89.83914% with no
frame and 141/144 relocations. Keep the current narrow headers and declaration order pinned, and do
not recombine these axes before a newly retained predecessor/shared-header state exists.

The integrated-root `76a78cc` pointer audit added four states. Top-level `volatile` or `const`
changes the CodeView-proven pointer symbol, while function-wide and setup-only reference aliases
remove the frame residue and four required relocations. These reject the hypothesis that retail's
formal reload pattern comes from cv qualification or reference storage.

The fresh `9c7eb18` lifetime audit added source SHA-256
`2e6bb567c731745d39b20dba1c5c70bc013c6d21899e9a4ba0cc373c7d8b1162`. Initializing the
long-lived clip width through the optimizer escape hatch `clipW | 0` is byte-identical to the
canonical state: 93.72654%, candidate `0x58c`, four-byte frame, and exact 144/144 relocations. It
does not transfer clip width into EBP, release shear into ESI, or remove the fill-count spill, so
that identity spelling must not be retried while the canonical TU/header state agrees.

## BASE/Textntry fresh lifetime and semantic-name pass

`textntry-fresh-lifetimes-97c1152.tsv` records the independent pass from checkpoint `97c1152`.
It did not use either permutation tool. The decisive retail-evidenced lifetime was that the same
32-bit `shifted` flag controls both display-adjustment loops. Reusing that flag makes VC4.2
coalesce its non-overlapping lifetime with the unsigned-short cursor in EBX, assigns `this` to EBP,
and produces a raw-exact `SetupDisplayString`: size `0x1be`, frame `0x130`, and 8/8 matching
relocation targets.

The matrix also records why reusing the cursor itself is structurally wrong: it reaches 99.73% but
emits `xor/test bx` where retail proves a separate 32-bit flag with `xor/test ebx`. Narrowing the
separate flag's lexical scope and adding dead initializers both reproduce the old 97.128380% state.
One constructor-local snapshot perturbed the later exact Setup function to 99.93%, directly proving
that exact-preserving predecessor/TU state must be audited after every earlier source change.

The retained source/header SHA-256 values are `ac8dd08884961ba6d73e8a312391cdeecac71ba132f5badef5f79201a63b34d8`
and `8207f088751e59f5a467c7a7e582c4870d82c3fb4e060262d02d5e3a07a17517`.

## BASE/Textntry post-exact-Setup constructor and Read AST passes

The exact Setup lifetime changed the combined TU state, authorizing one fresh pass over each of the
two remaining Textntry residuals from checkpoint `2aeff38`. Both used only
the then-current `scripts/permute_ast.py`; the regex permuter was never used, unsafe inequality
rewrites were
disabled, and all previously listed Textntry hashes were skipped before compilation.

`textntry-ctor-ast-ac8dd.tsv` contains 286 new unique whole-file hashes from seed `28482744` and 20
first-order variants. It has SHA-256
`aae422221e02fcdce50fbf103aa918624b0e01b5c97f62287e6061772a9bf873`. Thirty-eight hashes reached
98.695656%, but the final score-max representative `4363f3b...` failed the required raw target
review: it moved `m_color` (`+0x28`) ahead of the other constant stores. That made the score rise
while regressing an already-correct span, so it is explicitly marked `REJECT_RAW_STORE_ORDER` and
the canonical `ac8dd088...` source was restored.

`textntry-read-ast-ac8dd.tsv` contains 303 new unique hashes from seed `2848920` and 23 first-order
variants. It has SHA-256
`6399eacb8b875aefe21feb91aee2033692dca88a71f02f78ca76047b69613bdc`. Of these, 240 kept every
pin and 63 are marked `PIN_BYTES_MOVED` for the `Main` disassembly fingerprint/placement. No
pin-clean state exceeded 98.675500%.

The byte pins covered default constructor/destructor, `Main`, `Draw`, and the now-exact Setup; the
`Read` walk also pinned the long constructor. The accepted canonical source/header hashes remain
`ac8dd08884961ba6d73e8a312391cdeecac71ba132f5badef5f79201a63b34d8` and
`8207f088751e59f5a467c7a7e582c4870d82c3fb4e060262d02d5e3a07a17517`.

## BASE/WINDOW whole-TU jump-table revalidation

`window-manual-tu-72ca327.tsv` records the canonical 15-function TU and one fresh
exact-predecessor state. Its SHA-256 is
`7619528c745728ad6ad447c89e86fd8e7671437999744c9980ec0aed390e627d`. Neither permutation
tool was used.

The canonical source SHA-256 is
`b286e1136d4692eb131fefc4c5d392b39ea8401338196a9ba0a79c8553194ac3`. Fourteen functions,
including both constructors preceding the resource constructor, are exact. For
`heroWindow::heroWindow(int,int,char*)`, candidate and retail are both `0x521` with a `0x68`
frame. Excluding the five-entry table at relative `+0x4d7..+0x4ea` and masking all 57 matching
relocation positions leaves 1,293/1,293 equal bytes. Explicit-range disassembly gives identical
EBP displacements and one `e9 00000000` inline continuation on each side. The external relocation
targets agree; the extra delinked target identities are folded local table/case labels.

The sole non-table raw difference remains the dispatch relocation addend: MSVC emits
`ff 24 85 00000000` against its internal `$L` table symbol, while the delinked retail object emits
`ff 24 85 d7040000` against the containing constructor. The five table entries agree after masking
their local-label relocations. This is the byte-proven mid-function jump-table artifact documented
in `docs/jump-tables.md`, not a source/CFG/frame/slot/inline mismatch.

Renaming all five parameters of the preceding exact constructor produced source SHA-256
`7185388d9a86cfd2ebc976baafa621545d475ca6d2c0894eb561db308c6ca935` but left the complete TU
`.text` byte-identical at SHA-256
`b9c24bc77e7c2879084e1d697cd31ba6e8e557769dcd88a9364eabc21d43bde9`; all 14 siblings and the
99.916664% target stayed pinned. It was reverted. Do not repeat this parameter-name predecessor
state while the canonical source/header state agrees. An AST pass is not applicable to the
remaining residual because every non-table code byte is already exact and source AST mutations
cannot change the delinker's folded relocation addend.

## BASE/FONT scalar-lvalue SIB resolution

`font-sib-lvalue-72ca327.tsv` records the complete FONT continuation from checkpoint
`72ca327`; its SHA-256 is
`1095e413c0316e9a84fc23119350bbf4d942ec9c354f1f008b83667c19b68aa8`.
Neither permutation tool was used. Full source/header hashes and every ineffective or retained
source shape are recorded so the direct relation, `maxW | 0`, and `0[&maxW]` forms are not
repeated.

The successful form is the scalar-lvalue SIB identity, now spelled `OD_STEER(local)`
(the `va.h` macro expanding to `0[&(local)]`). It makes VC4.2 `/Od` load the
other comparison operand first without changing semantics, slots, instruction count, CFG, or
relocations. Applying it to the two index/start comparisons in `DrawBoundedString`, the three
width/argument comparisons in `LineLength`, and the length/index comparison in `LineWidth` makes
all three functions raw-exact. The prior stale `@early-stop` markers on the first two are removed.
The reusable byte-level before/after form is documented in
[`tu-cumulative-eval-order.md`](../patterns/tu-cumulative-eval-order.md).

The two deleting-destructor report rows remain unscored because delinking materializes two retail
`??_E` aliases while VC4.2 correctly emits one `0x39` `??_G` COMDAT plus a weak `??_E -> ??_G`
external. Manual raw review proves the COMDAT identical to each retail copy over all `0x39` bytes;
both relocations target `font::~font` and `operator delete`, and the vtable relocates through the
weak `??_E` alias. This is a report/delink alias artifact, not missing source.

## BASE/Textntry manual TU-state and lifetime pass

`textntry-manual-tu-a4fa3a0.tsv` records 35 new unique manual states from integrated checkpoint
`a4fa3a0`. Its SHA-256 is
`eba42858cfad92679dfe1736d6ab31656064d1029f5f6bae88a177f6984cf2ec`. Neither permutation tool was
used. Each row records the full source SHA, focused scores, raw target outcome, sibling outcome, and
disposition; every state was checked against all older Textntry matrices before compilation.

The pass covers exact-neutral default-constructor, long-constructor, and destructor spellings;
constructor parameter/member lifetime aliases; width and storage-class variants of `Read`'s
`enabled`; three progressively factored duplicated-arm CFGs; and two combined exact-predecessor
bundles. No state was retained.

The important new boundary is hash `9704ae26...`: `short enabled` emits retail's desired
`cmp edi,3` before the `m_height` load, but changes the following constant from retail
`mov eax,1` to candidate `mov ax,1`. Hash `9fe21062...` proves that `enabled | 0` is optimized back
to the same wrong-width instruction. Short-to-int snapshots add `movsx`; using the short in the
final predicate adds an extra sign-extend/subtract/compare sequence. The duplicated-arm family
leaves real extra branches and must not be repeated.

The canonical source/header hashes remain `ac8dd08884961ba6d73e8a312391cdeecac71ba132f5badef5f79201a63b34d8`
and `8207f088751e59f5a467c7a7e582c4870d82c3fb4e060262d02d5e3a07a17517`.

## BASE/Textntry exact-preserving TU-state continuation

`textntry-tu-state-ab91486.tsv` records seven new manual states from checkpoint `ab91486`. Its
SHA-256 is `9531574071c6fa3434afa2f2ca38e429718983d114541c66f462f712b34ec6c1`.
No permutation tool was used. The matrix carries both full source and header hashes because two
states change only the ABI-neutral declaration order in `textEntryWidget.h`.

The source-lifetime probes move `Read`'s 32-bit `enabled` declaration to function and outer-branch
scope, hoist the constructor's existing `loadedIcon` and `rectX` declarations without moving their
initializations, spell the exact destructor call through an explicit resource-manager dereference,
and combine all three byte-neutral forms. The header probes swap the two non-virtual method
declarations and move both non-virtual declarations before the virtual-method block without
changing virtual order or class layout.

Every state preserves the complete nine-row Textntry objdiff vector, including exact default
constructor, destructor, and `SetupDisplayString`, as well as the existing `Main` and `Draw` scores.
The two header states also leave the rebuilt `BASE/WINDOW` and `BASE/Misc` units unchanged in the
status report. Raw review still shows only the constructor's delayed `m_iconFrame` store and
`Read`'s adjacent compare/height-load reversal. Do not repeat these lifetime, dereference,
declaration-order, or combined forms while the canonical source/header hashes remain
`ac8dd08884961ba6d73e8a312391cdeecac71ba132f5badef5f79201a63b34d8` and
`8207f088751e59f5a467c7a7e582c4870d82c3fb4e060262d02d5e3a07a17517`.

## BASE/Textntry exact-preserving include and lifetime combinations

`textntry-tu-state-d9694ae.tsv` records the fresh cross-lane pass from checkpoint `d9694ae`. Its
SHA-256 is `586dccc8df84b2b89692b0ffc1a882229f2c15c1ce9262e6fc81fbd581bc25da`.
Neither permutation tool was used. The matrix records full source and header hashes, the complete
nine-row Textntry score vector, and the raw or sibling rejection for every compiled state.

Four previously untested include-order states were byte-neutral. Direct constructor icon and
rectangle spellings were also individually neutral, but combining both regressed `Main` to
98.689080% without moving the constructor store. Semantic constructor parameter names were neutral
in the definition; adding those names to the header declaration changed only the later cumulative
TU state and regressed exact `SetupDisplayString` to 99.932434%. A header-level `typedef enum` used
for `Read`'s 32-bit flag produced the same Setup regression while leaving the `Read` schedule
unchanged, so it is not a valid reason to change the proved short field storage or introduce an
enum here.

The natural direct-literal and single-initialized-`int` Read forms both entered the known early-flag-
store family at 96.523180%; the latter also regressed `Main`. Opposite polarity for the duplicated
32-bit condition was byte-neutral. Combining that polarity with a neutral include order, direct
icon store, and the prior neutral header-method swap still preserved every sibling but did not move
either residual. This confirms that combined predecessor/TU-state variants must be measured, while
also closing these specific combinations. The canonical source/header hashes were restored to
`ac8dd08884961ba6d73e8a312391cdeecac71ba132f5badef5f79201a63b34d8` and
`8207f088751e59f5a467c7a7e582c4870d82c3fb4e060262d02d5e3a07a17517`.
