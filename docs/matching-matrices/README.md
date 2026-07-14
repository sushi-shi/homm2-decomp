# Matcher experiment matrices

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

## BASE/Textntry AST searches

All three searches used only `scripts/permute_ast.py`, never the regex permuter, and required all 11
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

`textntry-setup-ast-341e775.tsv` records a focused `scripts/permute_ast.py` pass from integrated
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
`scripts/permute_ast.py`; the regex permuter was never used, unsafe inequality rewrites were
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

The successful form is the scalar-lvalue SIB identity `0[&local]`. It makes VC4.2 `/Od` load the
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
