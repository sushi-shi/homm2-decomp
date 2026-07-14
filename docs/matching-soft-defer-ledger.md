# Matching search no-repeat ledger

This ledger records exhausted or superseded value-preserving searches whose results must survive restarts.
It is negative knowledge: the entries are **not** accepted compiler walls and must not receive
`@early-stop`. They are temporarily deferred only until a stated non-local retest trigger changes
the compiler state or provides new retail type/lifetime evidence.

Before repeating an experiment, compare the listed canonical commit and source/header hashes. If
they still agree, do not repeat the axis. If a shared header, predecessor, declaration order, or
reconstructed type changes, rebuild the target and retest from the first raw-byte divergence.

General rules:

- Preserve every already-exact predecessor and sibling byte-for-byte, including relocations.
- Record a distinct source hash and outcome for every retained or rejected source-state variant.
- Do not normalize operands, registers, stack displacements, or external relocation identities.
- Use `scripts/permute_ast.py` only after semantics, size, frame, slots, and CFG align at roughly
  96-97%. Never use the regex permuter for these targets.
- A clean soft defer is a scheduling decision, not proof that the remaining bytes are impossible.

## BASE/Textntry: remaining constructor and Read residuals

Status: active. `SetupDisplayString` is now byte-exact. The delayed constructor store
and the adjacent `Read` scheduling reversal do not meet either permitted `@early-stop` flavor. They
remain unmarked and in the same lane.

Canonical source state:

- checkpoint: `a4fa3a0`
- `src/BASE/Textntry.cpp`:
  `ac8dd08884961ba6d73e8a312391cdeecac71ba132f5badef5f79201a63b34d8`
- `include/BASE/textEntryWidget.h`:
  `8207f088751e59f5a467c7a7e582c4870d82c3fb4e060262d02d5e3a07a17517`
- long constructor, RVA `0xd87b0`: live 98.652176% with the prior 98.695656% retained maximum,
  base/retail size `0x134`, 6/6 relocations;
- `Read`, RVA `0xd8920`: 98.6755%, base/retail size `0x26c`, 52/52 relocations;
- `SetupDisplayString`, RVA `0xd9570`: 100.000000% raw-exact,
  base/retail size `0x1be`, frame `0x130`, 8/8 relocations.

The three constructor constant member stores now occur in the unmasked retail offset order; the
only remaining raw scheduling residual is retail storing `iconFrame` before them while the candidate
delays that independent store. `Read` differs only by retail comparing `type` before loading
`m_height`, while the candidate emits those instructions in the opposite order. The exact Setup source
reuses one 32-bit `shifted` flag across both display-adjustment loops, which gives retail EBP=`this`,
EBX=cursor/flag allocation and the proved 32-bit clears/tests in the dead second-loop tail.

### Searches already exhausted

- constructor member-store and constant-flag order was steered until only the one delayed parameter
  store remained;
- `Read` default-rectangle and shared-enabled-value shapes were steered until only the adjacent
  compare/load reversal remained;
- two independent 220-iteration exact-preserving TU-state searches were run from the newly improved
  constructor and `Read` source hashes;
- every search candidate pinned all 11 sibling functions, and neither search improved
  `SetupDisplayString`;
- destructor aliases were tested only with ABI-valid source forms; none improved them;
- no diagnostic scripts or generated status changes were retained from the searches.

AST-gated searches after `e4e96c0`:

- initial constructor pass: 562 unique source hashes; found and retained the 98.695656% state;
- `Read` pass from the improved constructor: 356 unique source hashes; no improvement;
- deep constructor continuation: 819 unique source hashes; no further improvement;
- every mutation was produced by `scripts/permute_ast.py`, all 11 siblings were pinned, and the
  regex permuter was never used;
- the exact hash/score rows are persisted under
  [`docs/matching-matrices/`](matching-matrices/README.md) and must be checked before another AST
  walk at the same source/header state.

New manual `SetupDisplayString` and predecessor states:

- Setup-equivalent 95.0% hashes: `3d218f`, `d6275b`, `0ac643`, `7ba293`, `e5e564`;
- exact default-constructor/Main predecessor hashes leaving Setup unchanged: `534ee6`, `56260e`,
  `3ff8a1`, `750047`, `601ce9`, `66fac8`, `629715`, `2dbe13`, `c7f37d`;
- `a3fe00` left Setup unchanged and regressed Main to 99.542015%;
- `da8df8` over-optimized Setup to 89.62838%, size `0x1ac`, frame `0x130`, 8/8 relocations.

Fresh retail-structure pass after `2e351c8`:

- moved the `field_0x4b > 0` guard outside the second `do` loop and restored the explicit
  zero-offset flag clear proved by retail; this raised Setup from 95.000000% to 97.128380% and
  matched retail size `0x1be`;
- plain and `register` cursor snapshots were byte-identical;
- separate early loop-flag declarations and both combined declaration orders were byte-identical;
- all siblings remained at the pinned baseline for every state;
- exact whole-file hashes and outcomes are in
  [`docs/matching-matrices/textntry-setup-fresh-2e351c8.tsv`](matching-matrices/textntry-setup-fresh-2e351c8.tsv);
- neither permutation tool was used in the structural pass. Setup now meets the per-function
  96-97% AST gate with exact size, frame, CFG, and relocations, so a new libclang AST search is
  authorized from this genuinely changed canonical state; the regex permuter remains forbidden.

Focused Setup AST pass after integration at `341e775`:

- `scripts/permute_ast.py` exposed 18 legal first-order variants and compiled 286 new unique hashes
  in a deterministic 300-attempt walk;
- every candidate remained at 97.128380% with all 11 objdiff symbols pinned;
- existing matrix hashes were skipped, unsafe inequality +/-1 rewrites were disabled, and the
  regex permuter was never used;
- the complete no-repeat set is
  [`docs/matching-matrices/textntry-setup-ast-341e775.tsv`](matching-matrices/textntry-setup-ast-341e775.tsv).

Focused constructor and `Read` AST passes after exact Setup integration at `2aeff38`:

- the exact Setup lifetime materially changed the canonical TU, so both residuals received one new
  syntax-aware pass even though older Textntry matrices existed;
- the constructor walk used seed `28482744`, exposed 20 first-order variants, skipped 1,949 old
  Textntry hashes before compilation, and recorded 286 new unique hashes;
- 38 constructor hashes reached the retained 98.695656% maximum, but the retained representative
  `4363f3b877bae06621366e5e1b64eed03f687dd29af5446ce0c1f456b3984cb7` was rejected after raw
  review: it emitted the `m_color` (`+0x28`) store before the `+0x45/+0x47` stores and therefore
  regressed an already-correct store-order span while still delaying `m_iconFrame` (`+0x2f`);
- the canonical `ac8dd088...` constructor was restored because it preserves the retail order of all
  three constant stores and differs only by the already-documented delayed `m_iconFrame` store;
- the `Read` walk used seed `2848920`, exposed 23 first-order variants, skipped 2,235 old hashes,
  and recorded 303 new unique hashes; 240 passed every raw pin and 63 were rejected when the
  `Main` disassembly fingerprint/placement moved;
- none of the accepted `Read` candidates exceeded 98.675500%, so the sole raw residual remains the
  adjacent retail `cmp edi,3` before `m_height` load versus the candidate's reversed schedule;
- both passes used only `scripts/permute_ast.py`; unsafe inequality rewrites and the regex permuter
  were disabled. Default constructor/destructor, `Main`, `Draw`, and exact Setup were byte-pinned;
  the `Read` pass additionally byte-pinned the accepted long constructor.

Every full hash, score, and pin/rejection outcome is in
[`textntry-ctor-ast-ac8dd.tsv`](matching-matrices/textntry-ctor-ast-ac8dd.tsv) and
[`textntry-read-ast-ac8dd.tsv`](matching-matrices/textntry-read-ast-ac8dd.tsv). Do not repeat these
hashes while the canonical source/header hashes remain `ac8dd088...`/`8207f088...`. A higher fuzzy
score alone is not a retest trigger and must never override an earlier raw-span regression.

Manual lifetime, CFG, and exact-predecessor pass after integration at `a4fa3a0`:

- 35 new unique whole-source hashes were compiled or classified without either permutation tool;
  the canonical source/header hashes were restored unchanged;
- exact-neutral predecessor forms covered explicit `this`, nested scopes, chained/comma stores,
  `const` constructor locals, `register` on `iconFrame`, `iconFrame | 0`, `0[&iconFrame]`, and two
  combined exact-predecessor bundles. Eighteen states left both residuals unchanged;
- constructor member/parameter reference aliases changed already-correct raw spans and regressed
  exact Setup to 99.932434%; explicit storage casts, a constructor-base-argument cast, and a
  destructor pointer snapshot also regressed Setup without helping either target;
- changing only `Read`'s branch-local `enabled` from `int` to `short` moved `cmp edi,3` before the
  `m_height` load exactly as retail, while preserving exact Setup and every pinned sibling. It is
  still invalid: candidate emits `mov ax,1`, while retail requires `mov eax,1` before the two word
  stores. Read scores 98.642390% in that state;
- `enabled | 0` is folded back to the same wrong 16-bit materialization. A short-to-int snapshot
  adds `movsx` and regresses Setup; reusing the short in the final compare adds sign extension,
  subtraction, and another compare. `long`, `unsigned int`, and `register int` are byte-neutral and
  retain the original load/compare reversal;
- duplicating the common `type == 3` arm at progressively later rectangle-store boundaries does not
  reproduce retail commoning. VC4.2 retains extra branch bodies/compares, scoring 94.390730% through
  98.013245%, so do not revisit the duplicated-arm CFG family;
- the complete rows are in
  [`textntry-manual-tu-a4fa3a0.tsv`](matching-matrices/textntry-manual-tu-a4fa3a0.tsv).

The 16-bit `enabled` result is a diagnostic boundary, not an accepted improvement: the schedule is
right only by changing the proved 32-bit constant materialization. Neither residual is a permitted
wall and neither receives `@early-stop`.

Exact-preserving TU-state continuation after checkpoint `ab91486`:

- seven additional states were rebuilt with full source/header SHA-256 values and the complete
  nine-row Textntry objdiff vector recorded in
  [`textntry-tu-state-ab91486.tsv`](matching-matrices/textntry-tu-state-ab91486.tsv);
- moving the existing 32-bit `Read::enabled` declaration to function scope or immediately before
  the outer `type == 2` branch is byte-neutral and leaves the compare/load reversal unchanged;
- hoisting the constructor's existing `loadedIcon` and `rectX` declarations without moving their
  initialization sites is byte-neutral and leaves the delayed `m_iconFrame` store unchanged;
- spelling the exact destructor call through `(*gpResourceManager).Dispose(...)` is byte-neutral,
  and combining that spelling with both lifetime changes still leaves both target residuals
  unchanged. This closes that combined predecessor/TU-state bundle, not just its individual forms;
- swapping the two non-virtual member declarations, and separately moving both non-virtual method
  declarations ahead of the virtual-method block while preserving virtual order, are ABI-neutral
  header-state probes. Both preserve all Textntry rows and the rebuilt `BASE/WINDOW` and
  `BASE/Misc` unit scores, but neither changes either residual;
- no permutation tool was used. In particular, this continuation does not reopen the exhausted
  libclang AST matrices and never uses the regex permuter.

The canonical source/header hashes remain `ac8dd08884961ba6d73e8a312391cdeecac71ba132f5badef5f79201a63b34d8`
and `8207f088751e59f5a467c7a7e582c4870d82c3fb4e060262d02d5e3a07a17517`.

Fresh semantic-lifetime pass from lane checkpoint `97c1152`:

- reusing the same 32-bit `shifted` local across both loops makes `SetupDisplayString` raw-exact;
- reusing the unsigned-short cursor instead proves the right allocation but leaves four wrong-width
  `xor/test bx` instructions, so that tempting 99.73% state is structurally wrong and was reverted;
- narrowing a separate `int` flag and initializing it with zero or cursor are byte-identical to the
  old 97.128380% state;
- a constructor-local icon-frame snapshot regresses the later Setup allocation to 99.93%, proving
  predecessor/TU-state sensitivity; all hashes and outcomes are in
  [`textntry-fresh-lifetimes-97c1152.tsv`](matching-matrices/textntry-fresh-lifetimes-97c1152.tsv).

Do not repeat those TU-state searches while the canonical source hash and sibling hashes agree.
The newly authorized local Setup AST space is now exhausted. Continue only with a retail-evidenced
lifetime or a genuinely different exact-preserving predecessor/TU state. Any later accepted wall
must satisfy the two narrow rules in `.claude/agents/matcher.md`; the remaining constructor and
`Read` scheduling residuals do not.

## BASE/listbox: listBoxWidget::Main

Canonical source state:

- checkpoint: `e2c6822`
- target: RVA `0xdb520`
- `src/BASE/listbox.cpp`: `a81adc0db8c9c8e20995a5f123cf218cd8746974d728d677cc1ce95c0ee4782f`
- `include/BASE/listBoxWidget.h`: `9c4035d2d94d3774180463006ade94348cd431251c88a599f73487cb285ea73b`
- `include/BASE/widget.h`: `1a2343d9a53626c243d47b45d90b83968356a9abd5364a6e52cd153d71228fe5`
- `include/BASE/heroWindow.h`: `14a00e5b0af8465d27af8a23f1c054c42e353109c1bd1ae24b9dee7a16f5fdd3`
- `include/_carcass_types.h`: `27c231717cbc6d73835ec4b27c00568d3384d4aa780c8c87bdcbcaef3fa7a6c3`

The candidate and retail functions are both `0x368` bytes with the same frame, CFG, and 23
relocation sites. `Read` and `ProcessMouseMessage` are raw-exact. The only non-local-label code
difference is the operand order and branch polarity at Main `+0xa5/+0xa8`:

```text
candidate                         retail
66 3b 55 1a  cmp dx,[m_y]         66 39 55 1a  cmp [m_y],dx
7c ..        jl  ...              7f ..        jg  ...
```

The relation is equivalent, but these operand bytes are real and are not a permitted delinker
artifact. The remaining jump-table identities are local-label naming differences.

### Searches already exhausted

Local and binding-aware axes:

- two nested lexical-scope schemes around `Read`'s five scalar locals;
- 20 single and 300 multi binding-aware local-name variants with all 11 symbols pinned;
- Main `mx`/`my` declaration order, split initialization, and widened scopes;
- direct relation spellings plus `*(&m_y)`, `my | 0`, and `0[&m_y]`;
- no unsafe inequality mutation was retained.

Exact-preserving TU/source states, all keeping the seven exact listbox functions exact while Main
kept the same `3b/7c` signature:

- Draw explicit-`this` call: `0180a5a1`
- DrawLBStuff widened `i` scope: `0964b524`
- Process split `firstRowHeight` initialization: `92726519`
- Process split coordinate/`adjY` initializations: `facd6953`
- font/icon include order: `070119e5`
- early `heroWindow` declaration: `ae6a1b36`
- early KB/`tag_message` owner declaration: `efec2a6d`
- early `X_GLOBAL` declaration: `238969a8`

Exact-preserving header states, again leaving Main unchanged:

- widget `m_y` explicitly signed: `4f2fcfd0`
- widget `m_x` explicitly signed: `cab45d4b`
- both widget coordinates explicitly signed: `aeaca177`
- heroWindow `m_posX` explicitly signed: `969c7b2e`
- heroWindow `m_posY` explicitly signed: `7a1acda9`
- both heroWindow positions explicitly signed: `02805793`
- list geometry short/int declarations: `e44aea5a`
- owner-method signed-int declarations: `b3fd2570`
- split `tag_message` fields: `1d31335a`

Rejected because an exact sibling changed raw code:

- DrawLB initialized x/y: `e1e057f1`
- offset conditional expression: `3b95a99a`
- explicit up-frame branch: `82c8bb74`
- explicit down-frame branch: `15729643`
- promoted `firstRowHeight` to int: `b9bcb088`

No coordinate accessor was invented because retail/header evidence does not support one.
The temporary binding-aware AST tooling was fully restored; canonical
`scripts/permute_ast.py` SHA-256 was
`a3e4268e...`. The regex permuter was never invoked.

### Retest trigger

Do not repeat these axes while the listed hashes agree. Retest Main after a real shared
widget/window/message type change, a source-order change in an exact predecessor, or another
combined-TU change that alters MSVC state. Re-run raw checks for ctor, dtor, Delete, Read, Draw,
DrawLBStuff, and Process before accepting any downstream gain.

## BASE/icon2bc: IconToBitmapColorTable

Canonical source state:

- checkpoint: `caa9528`
- target: RVA `0xd32a0`, retail size `0x5af`
- `src/BASE/icon2bc.cpp`: `f060c410fd0e8bd0140d2ce4e83c70236ef3d936fe5b52366003b82e563a4ba0`
- live checkpoint: 71.8803%, 88 candidate vs 91 retail relocations, no base-only target

This is a single-function TU, so predecessor steering is unavailable. The remaining mismatch starts
in setup and forms one root register-allocation/CSE cascade. Retail keeps the frame entry, X
temporary, and saved destination cursor as distinct lifetimes; the candidate folds the entry and
colors the saved cursor differently:

```text
candidate setup                    retail setup
lea ecx,[eax+4*(eax+2*eax)]        lea ebx,[eax+4*(eax+2*eax)]
add ecx,esi                        movsx ecx,word ptr [ebx+esi]
mov eax,[ecx+srcOffset]            mov eax,[ebx+esi+srcOffset]
                                   lea edi,[ebx+esi]
```

At the checkpoint, retail is about 72 bytes longer. The measured deficits were concentrated in
setup/dispatch (~23), fill (~33), dim (~11), and literal (~6). The three missing relocation sites
correspond to redundant retail CSE anchors:

- the skip path restores/stores `gCTDst`;
- the fill path reloads `gCTClipR`;
- the dim path duplicates a `gCTCnt` store.

### Corrections retained at the canonical checkpoint

- file-static scratch declarations are in ascending retail address order;
- saved scratch state is `gCTDst`, not `gCTDimLen`;
- `gCTCnt` and `gCTCnt2` have their retail-proven roles;
- dim and literal output reuse one saved/output cursor lifetime;
- literal count and destination are formed only after clip tests;
- the clipped literal predicate preserves retail's second `gCTDst` restore;
- nonexistent per-pixel `gCTDst` updates were removed;
- the invalid generic `@early-stop` was removed.

These corrections raised relocation coverage from 86 to 88 of 91 and made the literal block
essentially retail length. They are the base for future work; do not restart from the older shape.

### Searches already exhausted

Approximately twenty manual, value-preserving source-shape families were measured and reverted
when they failed to beat the checkpoint:

- all six persistent-entry S/X/E statement orders with a separately declared X temporary
  (best 71.74%);
- saved cursor declared before/after entry, after row construction, and loop-scoped
  (best 71.86%);
- explicit `entryX` and `srcOffset` lifetimes (66.68-71.10%);
- `register` hints, which VC4.2 ignored;
- full direct-entry simplification (69.05%) and one-alias simplification (71.88%);
- each redundant CSE anchor in isolation and in combination;
- dim duplicate-store alone or combined anchors triggered a 63.75% allocation cascade;
- skip `gCTDst` store alone reached 71.04%;
- a direct `IconEntry` header/declaration state emitted all 91 relocations but fell to 67.85%
  with wrong compare/register scheduling;
- keeping the original `IconEntry` declaration position from `Misc` remained best at
  71.88% with 88 relocations.

No AST permutation was run because the function is far below the 96-97% structural threshold.
The regex permuter was never used.

### Retest trigger

Do not repeat these axes while the canonical source hash agrees. Retest after a real shared
`IconEntry`, icon, bitmap, or related header/layout change, or after adjacent icon decoder work
reveals a retail-proven lifetime/type/source ordering not represented above. A retest must begin at
the setup divergence, preserve the retained cursor/literal corrections, and audit all 91 relocation
sites. The current residual is a soft defer, not a permitted wall.

## BASE/iconf2bc: FlipIconToBitmapColorTable

Status: active. This is an integrated progress checkpoint, not a wall or soft defer. The same lane
must continue from the measured width/X, row-input, and setup-`gFCY` divergences before taking
unrelated work.

Canonical source state:

- checkpoint: `48dbe3e`
- target: RVA `0xd9790`, retail size `0x54d`
- `src/BASE/iconf2bc.cpp`:
  `dd868c0ffcdabf134c98631a7a731d7b8d90845b7df496ce5a3c6dd05b305fd9`
- live checkpoint: 85.9925%, candidate size `0x541`, 84 candidate vs 83 retail relocations,
  no base-only target
- the command decoder begins at `+0xea` versus retail `+0xec`

### Corrections retained at the canonical checkpoint

- removed the invalid generic `@early-stop` and its unproved `/O2` wall claim;
- recovered the split X/width/entry setup lifetimes and retail clipping extent tests;
- rebuilt solid and dim runs with their retail body order, counters, cursor publications, and
  per-iteration palette reloads;
- rebuilt unclipped and four-quadrant clipped color-table literal loops;
- restored source advancement for fully clipped literal runs;
- restored the retail-evidenced branch-selected literal destination cursor, common skip
  publication, and literal-count snapshot lifetime;
- replaced broad/redundant owner and `Misc.h` includes with the narrow `IconEntry.h` declaration
  surface, removing the clipped fill/dim excess `gFCY` loads;
- declared width before the entry load and initialized X through `gFCXEnd`, restoring retail's
  EAX/EBX publication sequence and two of the three missing setup bytes;
- declared the later row pitch beside the entry cursor, restoring a separate `gFCY` load while
  preserving the retained width/X setup;
- reused the later pitch lifetime to preserve the original icon width while transforming `w` into
  the exclusive horizontal bound used by the clipping setup;
- moved the final row sum and `gFCRow` publication into retail's `eax`; the `gFCY` and destination
  pixel inputs still load into the opposite registers;
- corrected the `gFCClipR` lifetime/count to agree with retail;
- matched every external relocation target and every scratch-global occurrence count except
  `gFCY`.

The remaining measured relocation delta is exactly one redundant setup `gFCY` load. Every other
scratch-global occurrence count agrees. Candidate code ends twelve bytes before retail. These facts
are the next concrete steering target; they are not evidence that the residual is impossible.

### Searches already exhausted

The superseded pre-reconstruction wall had tried local/global/mutable setup, signed and unsigned
counts, merged and split `memset` shapes, pointer-update orders, global palette/destination loads,
and 136 historical AST variants over five rounds. Those permutations were run far below the
current 96-97% structural gate and must not be repeated.

The clean reconstruction passes measured setup-local forms, compare operand polarities,
function-wide and nested vertical-coordinate locals (including `register` spellings), width and
entry aliases, source-pointer spellings, dim-loop statement order, palette aliases, and literal
cursor forms. The latest pass additionally measured integer-domain source offsets, packed X/width
subtraction, width-as-exclusive-bound lifetimes, early and split Y publication, transformed-bound
pitch reuse, and several original-width alias scopes. Losing variants and useful partial wins that
regressed in combination were reverted; the retained form is the canonical checkpoint above. No
permutation tool was used, and the regex permuter was never used.

The complete source-hash matrix, including every measured score, size/frame, relocation count, and
retained/reverted/byte-identical disposition, is in
[`docs/iconf2bc-experiment-matrix.md`](iconf2bc-experiment-matrix.md). Treat that file as the
authoritative no-repeat list for this source/header state.

Fresh cross-lane setup/lifetime batch after integrated root `76a78cc`:

- load the packed icon width directly into the later pitch local while keeping `w` bound-only:
  85.84131%, end `0x544`, frame eight, decoder `+0xed`, 84/83 relocations. This restores the
  retail row-input load order but publishes the row in `ecx`, loses the retained width/X order,
  and never materializes the early width spill; reverted;
- make the reused icon-width/destination-pitch local volatile: 85.561714%, end `0x54a`, frame
  twelve, decoder `+0xf3`, 84/83. The width materializes, but in a third frame word with broad
  setup/decoder allocation changes; reverted;
- order the file-static decoder scratches by first semantic use: 84.95214%, end `0x545`, frame
  eight, decoder `+0xe6`, 85/83; reverted. This single evidence-based order regresses, so broad
  scratch declaration permutations remain out of scope;
- commute the row sum: byte-identical to 85.99245%. A near-use pixel snapshot scores 85.94206%,
  and staged pixel/Y snapshots in retail source load order score 85.60201%; all still compile to
  candidate `gFCY -> ecx`, pixels -> `eax`, followed by the already-correct `eax` publication;
  reverted;
- commute the first setup vertical comparison, assign its Y snapshot in that clause, or rename
  the reused pitch lifetime to semantic `width`: each byte-identical to 85.99245%, end `0x541`,
  frame eight, decoder `+0xea`, 84/83 with nine `gFCY` occurrences; reverted.
- chain the packed-width load as `pitch = w = entry->w`: source SHA-256
  `fa4d6d949d4a6903b5a62c5707335d084ec1a784e649eec35f3b708f373563d3`, 85.16121%, end
  `0x540`, frame eight, decoder `+0xe9`, 84/83 with nine `gFCY` occurrences. This does not emit
  the retail `[esp+0x14]` width spill and broadly regresses the setup allocation; reverted;
- reverse that chain to `w = pitch = entry->w`: source SHA-256
  `30f1335677be4de2cbbcae38f912ef0a8206e20f01fccb91486b81bdfff203c4`, 85.84131%, end
  `0x544`, frame eight, decoder `+0xed`, 84/83 with nine `gFCY` occurrences. This returns to the
  already documented direct-pitch structural state: row inputs load in retail order, but the row
  publishes in `ecx`, width/X remains wrong, and the early width still is not spilled; reverted.

The canonical source hash remains `dd868c0ffcdabf134c98631a7a731d7b8d90845b7df496ce5a3c6dd05b305fd9`.
The matrix now includes these eleven additional full hashes and has SHA-256
`aa9de67a6acca3f1461749491bfe5411be1607fd9dbb246086d9f1342d8c8f8b`. Do not recombine these
setup and row-load axes until a newly retained header/predecessor state changes the canonical TU.

Continue specifically from the first width/X load-order divergence, the swapped row-input register
loads before the now-correct `eax` publication, and the extra setup `gFCY` load.
Record each new source-hash-distinct shape with its match, size, frame, and relocation result. Do
not retry the families above while the canonical source hash agrees. If a shared icon/header edit
is retained, retest the deferred icon2bc, Icon2b, and Iconf2b functions as well.

## BASE/iconf2by: FlipIconToBitmapYModify

Status: active. This is not an accepted wall and must not receive `@early-stop`; the lane remains on
the prologue register-lifetime divergence.

Canonical source state:

- checkpoint: `813cf59`
- target: RVA `0xd9ce0`, retail size `0x58d`
- `src/BASE/iconf2by.cpp`:
  `f33ab84fe5e3b9205e40ae09de314bced82063fe587bddee30646c73d44bfea0`
- live checkpoint: 93.72654%, candidate size `0x58c`, 144 candidate vs 144 retail relocations,
  no candidate-only target
- the decoder aligns instruction-for-instruction through extended-run setup;
- candidate and retail both reserve a four-byte frame slot; the candidate still spills the fill
  count there while retail never accesses it

### Corrections retained at the canonical checkpoint

- removed the invalid generic `@early-stop` and its unproved `/O2` wall claim;
- recovered the explicit short/extended run-length branches and override run/color order;
- recovered the fill, dim, and literal quadrant polarity and body order;
- restored the dim destination join and publication order;
- restored literal source advancement/store order and right-clipped source publication before
  destination publication;
- introduced the real clip-width lifetime used by the clipped literal path;
- replaced the long-lived advance local with branch-selected source advancement and a common join;
- restored the right literal quadrant's zero-skip-first polarity and common computed-skip
  publication;
- recovered the unsigned fill-count snapshot that produces the retail-sized frame;
- narrowed that snapshot to the vertical-visible fill block, moving the `gFYRun` load from before
  the shear/vertical checks to the retail position after them;
- formed the right-quadrant destination once and published it in both arms, moving the global
  destination store between the compare and branch as retail does.
- recovered a setup-only promoted shear-value lifetime; it emits no setup-local instructions but
  changes the later literal-overlap compare to retail operand order and polarity.

Every scratch/global relocation count now agrees. The candidate still keeps `shear` in EBP while
retail pins `clipW`, and it still spills the fill count to the four-byte frame slot that retail
reserves but never accesses. These are concrete steering facts, not an accepted wall.

### Searches already exhausted

The superseded pre-reconstruction wall had tried signed/unsigned scratch lengths, local/global
flags, pitch, clip-width and advance forms, split/repeated clipping expressions, 124 historical AST
variants over eight rounds, 80 additional walks, and 60 text-shape variants. It also tested ordinary
overlap semantics for the fill/dim outer gates; that changed both compare sequences and fell to
73.58%. Those permutations were run far below the present 96-97% structural gate and must not be
repeated.

The clean reconstruction pass audited and rejected command and fill locals, function- and
branch-scope pointer locals, clip-edge publication order, signed/unsigned width and advance types,
combined clip predicates, explicit literal-failure joins, shared skip publication, loop comparison
polarity, setup-alias removal, and declaration/lifetime variations. Losing forms were reverted; the
retained forms are the canonical checkpoint above. Neither permutation tool was used.

Final measured axes after `8bd4149`:

- fill-only `currentY` scalar: byte-identical, 85.7239%, size `0x559`, no frame, 142/144
  relocations;
- `currentY` scalars across dim/fill/literal: 85.20%, size `0x559`, no frame, 142/144; reverted;
- right-edge scalar confined to the clipped-right arm: byte-identical;
- right-edge scalar retained across outer overlap and quadrant dispatch: byte-identical;
- `const` right-edge scalar: byte-identical;
- fill `currentY` plus widened right-edge lifetime: byte-identical;
- normalize the dead `clip` parameter to enabled and gate every path through it: 84.50%, size
  `0x55d`, no frame, 143/144; reverted;
- reuse the dead `clip` parameter as the clip-width scalar: 85.21%, size `0x559`, no frame,
  142/144; reverted.

Sibling-structure transfers measured after `e4e96c0`:

- `fillLeft` lifetime matching sibling `fillRight`: byte-identical;
- `dimLeft` lifetime matching sibling `dimRight`: byte-identical;
- inner `copyLeft` lifetime matching sibling `copyRight`: byte-identical;
- all three named edge lifetimes together: byte-identical;
- direct `clipW` plus retail/sibling right-first edge publication: 83.64%, size `0x559`, no frame,
  142/144 relocations; reverted;
- precomputed `clipRight` spanning initial Y/shear setup with canonical publication order:
  byte-identical;
- the same precomputed `clipRight` with right-first publication: 84.14%, size `0x559`, no frame,
  142/144 relocations; reverted.

The exact `icon2by` prologue naturally releases `shear` from EBP after its initial use and then
loads `clipW` into EBP. Transferring its named edge lifetimes does not change `iconf2by` allocation.
Pitch, palette/destination, selected-count, and fill-color locals are contradicted by this target's
already-correct relocation counts or duplicate earlier exhausted families.

Declaration-surface audit after `e07b643`:

- replacing broad `BASE/Misc.h` with narrow `BASE/IconEntry.h` while retaining the redundant own
  header: 86.782845%, 145/144 relocations;
- suppressing the `IconEntry` size assertion: 84.986595%, 142/144 relocations; reverted;
- direct `IconEntry` declaration with `Misc.h`: 78.0563%, 142/144 relocations; reverted;
- removing the redundant own-header include as well: 87.091156%, size `0x56d`, 145/144
  relocations; retained;
- narrow `X_GLOBAL`, `IconEntry`-first order, width rename, moved width declaration, direct
  `clipW`, `register` hint, widened `advance`, and string-first include order: byte-identical to
  87.091156%;
- widening all existing locals: 86.42091%; reverted;
- two retail-evidenced shared-skip joins: byte-identical and reverted to avoid source complexity.

Retail mangling proves ten integer clip/color arguments and a mutable signed-char shear pointee.
The packed `IconEntry` stride/field signedness and bitmap width type agree with retail access
opcodes, so no shared type edit is justified. The useful lever was declaration visibility, not a
layout change.

Source-advancement and frame axes after `b60f188`:

- explicit branch-selected advancement: 88.51%; `gFYClipR` exact but `gFYRun` 20/19,
  `gFYSkip` 5/4, and `gFYDimLen` 15/16;
- shared skip with the old right-quadrant polarity: byte-identical;
- common literal copy-count snapshot: 86.46%, 141/144 relocations; reverted;
- right zero-skip-first polarity: 88.83%, `gFYDimLen` 16/16 and `gFYClipR` 6/7;
- common skip publication in the corrected CFG: 88.91%, 144/144 relocations and `gFYSkip` 4/4;
- setup-only right edge, right-destination local, and split-destination local: byte-identical;
- signed fill snapshot plus right-lifetime form: byte-identical;
- combined current-Y/fill-count/fill-left snapshot: 86.73%; reverted;
- unsigned fill-count snapshot: 89.0885%, size `0x57a`, retail four-byte frame, 143/144
  relocations; retained;
- unsigned clip width: 88.02% with an eight-byte frame; reverted;
- scoped shear aliases: 88.89%; reverted;
- `register` and `const` width forms: byte-identical.

Destination-placement axes after `215f379`:

- one right-destination value formed before the quadrant test and published in both arms:
  89.101875%, retained;
- direct pointer-base, commuted subscript, commuted branch-common local, split edge-first local,
  right-count local, right-row/`register` local, and integer right-destination forms:
  byte-identical and reverted.

Shear-lifetime axes after `11424f1`:

- narrow initial shear pointer alias (`ebe47a72`) and wider alias scope (`3e15131b`): 89.47721%;
- smallest promoted-int initial shear snapshot (`5bcb2161`): 89.47721%, retained;
- direct `clipW` (`43d5e09f`), initial commuted shear subscript (`1944701e`), all commuted shear
  subscripts (`7ed76d08`), and `const` right destination (`36542cc2`): byte-identical to the prior
  89.101875% checkpoint;
- newline/literal/dim/fill pointer-alias extensions (`4b480715`, `2d490d76`, `f7e6d16f`,
  `b578d5c5`), right `| 0` identity (`cbaf2feb`), and commuted integer destination sum
  (`75be3b64`): byte-identical to 89.47721%;
- `const` setup pointer (`80af0503`), one-pass source publication (`4137c524`), sequenced comma
  statements (`0e668152`), comma initializer (`51edd169`), direct-init clip width (`b98385d1`),
  and `shear + 0` alias (`081d58de`): byte-identical to 89.47721%;
- signed-char value snapshot (`35688a84`), all-site value snapshots (`7fe672b0`), `register`
  right destination (`66ca2813`), `register` setup value (`f51119a9`), and split setup-value
  declaration/assignment (`33b353d4`): byte-identical to 89.47721%.

The authoritative full-SHA no-repeat set for the recovered manual states is
[`docs/matching-matrices/iconf2by-manual.tsv`](matching-matrices/iconf2by-manual.tsv). Later
right-block pointer/reference/local spellings were byte-identical or regressed; do not repeat them.
The earliest normalized divergence is the prologue register choice: candidate loads `shear` into
EBP at `+0x5c`, while retail loads `shear` into ESI and `clipW` into EBP at `+0x60`. The retained
scratch order closes the former downstream `gFYClipR` deficit. CodeView contains no symbol
subsection for this TU, so no local-name/type oracle exists beyond the retail instruction stream.

Retail's four-byte frame slot is never accessed: every ESP-relative retail access is an argument,
and after `sub esp,4` plus four pushes the smallest displacement is `0x18`. The slot is allocator
residue, not evidence for a missing local or padding variable. Neither permutation tool was used.

Final setup lifetime axes after `c131c56`:

- direct formal `clipW` with the canonical fill snapshot: 89.101875%, exact candidate size/frame,
  143/144 relocations; reverted;
- direct formal `clipW` with no fill snapshot: 89.11528%, size `0x567`, no frame, 145/144
  relocations; reverted;
- function-scope fill-count declaration: byte-identical to 89.47721%; reverted;
- `const int &` clip-width lifetime: 88.43163%, exact candidate size/frame, 143/144 relocations;
  reverted;
- moving the clip-width declaration immediately after current-Y formation: byte-identical to
  89.47721%; reverted.

Fill-lifetime and setup-order axes after lane base `1e1646a`:

- publish `gFYClipR` before the initial shear expression: 87.7319%, size `0x57b`, 143/144;
  reverted;
- move the unsigned fill-count snapshot inside the vertical-visible block: 90.402145%, canonical
  candidate size/frame, 143/144; retained because its `gFYRun` load now occurs after the vertical
  checks as retail does;
- keep that branch-local snapshot for clipping arithmetic but use `gFYRun` as the full-fill
  `memset` count: 88.793564%, size `0x563`, no frame, 143/144; reverted;
- signed branch-local fill count: 89.08847%, size `0x567`, no frame, 144/144; reverted;
- direct formal `clipW` after the branch-local recovery: 90.02681%, canonical candidate size/frame,
  143/144; reverted;
- publish the right-clipped dim length before computing the pending literal skip: 88.75335%, size
  `0x57d`, 143/144; reverted;
- named `fillLeft` after the branch-local count: 90.02681%, canonical candidate size/frame, 143/144;
  reverted.

The branch-local recovery also clarifies the four-byte-frame evidence: retail reserves but never
accesses the slot, while the retained candidate spills the fill count to it in the full-fill
intrinsic path. Fixing the ESI-shear/EBP-width allocation may remove that spill. The original NB09
stream has no symbol subsection at all for this compiland, so there are neither BPREL stack locals
nor optimized-register records to recover.

Count-storage and scratch declaration-order axes after lane base `f220572`:

- `register` branch-local fill count: byte-identical to 90.402145%; reverted;
- snapshot the count only in the full-fill arm: 89.08847%, size `0x567`, no frame, 144/144;
  reverted;
- make the global run scratch unsigned: 87.07775%, size `0x561`, no frame, 141/144; reverted;
- order file-static scratches by first semantic use: 91.89008%, size `0x583`, 143/144;
  superseded;
- transfer the adjacent `icon2by` scratch order: 91.675606%, size `0x583`, 144/144; reverted;
- move all first-use-ordered scratches to function-local static storage: 90.99196%, size `0x583`,
  143/144; reverted;
- move the clip-width declaration down to its first emitted setup use under the first-use scratch
  order: byte-identical to 91.89008%; reverted;
- retain first-use scratch order but declare `gFYClipR` last: 93.72654%, size `0x58c`, exact
  144/144 relocations; retained;
- also move `gFYY` last: 91.675606%, size `0x583`, 144/144; reverted;
- also move `gFYRun` last, or move `gFYXEnd` first: each byte-identical to 93.72654%; reverted;
- make only `gFYClipR` function-local static: 92.828415%, size `0x58c`, 144/144; reverted;
- use `gFYRun` as the full-fill `memset` count under the retained scratch order: 92.18499%, size
  `0x576`, no frame, 144/144; reverted.
- move `gFYClipB` beside the final `gFYClipR` declaration: byte-identical to 93.72654%; reverted;
- use formal `clipW` directly under the retained scratch order: byte-identical to 93.72654%;
  reverted, but this confirms why predecessor/TU-state retests must use whole-source hashes;
- use a `const int &` clip-width lifetime under the retained order: 92.68096%, size `0x58c`,
  144/144; reverted.

Combined setup, type, and header-state axes after integrated root `4d381e1`:

- publish clip-right before clip-bottom, either directly or through named edge temporaries, and
  transfer the sibling's complete direct-shear/direct-width setup: 92.544235%, size `0x58c`,
  frame four, 144/144; all reverted. These shapes recover retail's edge-store order but do not
  move `shear` out of EBP, so their later allocation is worse overall;
- direct initial shear, a `register` clip-width local, a precomputed clip-right spanning initial
  shear, `IconEntry` before `X_GLOBAL`, `const`/`unsigned long` fill counts, `register` or
  top-level-`const` clip-width formals, direct uses of a `register` formal, and commuted
  `gFYY[shear]` subscripts: each byte-identical to 93.72654%, size `0x58c`, frame four, 144/144;
  reverted;
- restore the owner header, or replace the narrow `IconEntry` header with `Misc`: each 84.28954%,
  size `0x56b`, frame four, 141/144; reverted. This proves the current narrow declaration surface
  must remain pinned;
- make the global run scratch unsigned under the retained order: 89.83914%, size `0x56d`, no
  frame, 141/144; reverted;
- use a non-const reference for clip width: 92.68096%, size `0x58c`, frame four, 144/144; reverted;
- spell the shear formal as an array: VC4.2 changes the emitted symbol instead of applying the
  standard adjusted-pointer signature expected here, so objdiff cannot claim the CodeView symbol;
  reverted.

Fresh shear-pointer type/lifetime audit after integrated root `76a78cc`:

- top-level `volatile` on the shear pointer changes the VC4.2 symbol from retail `PAC` to `RAC`;
  this is not an ABI-neutral qualifier and was reverted before scoring;
- top-level `const` on the shear pointer likewise changes the symbol from retail `PAC` to `QAC`;
  CodeView therefore proves the formal is a mutable, unqualified signed-char pointer;
- a function-scope `signed char *const &` alias for every shear access falls to 88.77%, size
  `0x563`, no frame, and 140/144 relocations; reverted;
- confining that reference alias to the initial shear calculation falls to 88.93% with the same
  size/frame and relocation regression; reverted.

The retail reload pattern is therefore not evidence for pointer cv-qualification or a reference
alias. Those shapes remove the retail four-byte allocator residue and four required scratch
relocations instead of transferring the pointer from EBP to ESI.

The full no-repeat set now contains 136 source-hash-distinct states. This combined-state batch
confirms that the retained declaration surface and scratch order are stable: the isolated
lifetime/type spellings are either byte-neutral, while retail-order edge publication still leaves
the same EBP shear allocation, or they regress code size/relocations. Do not combine these axes
again unless an independently retained predecessor or shared-header change produces a new
canonical TU state.

Fresh clip-width lifetime audit after integrated root `9c7eb18`:

- initialize the long-lived clip-width alias as `clipW | 0`: source SHA-256
  `2e6bb567c731745d39b20dba1c5c70bc013c6d21899e9a4ba0cc373c7d8b1162`, byte-identical at
  93.72654%, size `0x58c`, frame four, and exact 144/144 relocations. The optimizer still keeps
  shear in EBP, reloads the formal width later, and spills the branch-local fill count; reverted.

The authoritative matrix now has SHA-256
`f787d7905c05b7a28435c8bd9e6296f2075c7c4579f4cd744874c57330fdac5d`.

The retained order is not a random permutation: it declares the decoder scratches in first-use
order, with `gFYClipR` last as in the adjacent sibling's declaration surface. It closes the sole
relocation deficit and brings the candidate to one byte below retail size, but does not alter the
earliest EBP/ESI divergence or the fill-count spill. Do not replay broad declaration permutations;
continue with a newly evidenced lifetime or a retained predecessor/header-state change.

These close the obvious setup-lifetime spellings but do not prove a wall. Continue from the
prologue register divergence with genuinely new retail-evidenced structure, and do not run an AST
search while this function remains below its per-function 96-97% gate. A retained shared change
must also trigger iconf2bc, Icon2b, and Iconf2b retests.

## BASE/Icon2b: IconToBitmap

Status: clean soft defer pending a shared icon/header or compiler-state change. It is not an
accepted wall and must not receive `@early-stop`.

Canonical source state:

- checkpoint: `4498053`
- target: RVA `0xd0570`, retail size `0x4ed`
- `src/BASE/Icon2b.cpp`: `b42c0b8e368561082811dbcc7b170ab975483447470c577173ad6a9132f26124`
- live checkpoint: 74.4533%, 79 candidate vs 83 retail relocations, no base-only target
- candidate return at `+0x4cc` vs retail `+0x4ea`, with no stack frame on either side

### Corrections retained at the canonical checkpoint

- removed the invalid generic `@early-stop`;
- introduced the real setup Y lifetime, giving retail ESI/EDI data/entry allocation;
- used the retail global-source post-increment;
- recovered clipped fill/dim body order with `clipX <= X` tested first;
- recovered the three-call fill shape;
- rebuilt the literal path as branch-selected quadrants in a one-pass `do/while (0)`;
- ordered count/destination/source selection so every quadrant reaches one common memcpy tail;
- restored all six retail-style literal `gIcSrc` occurrences, including the unclipped tail.

### Searches already exhausted

The removed pre-reconstruction wall had already tried global/local X and entry forms,
source-read post-increment forms, relational reversals, split/common intrinsics, targeted volatile
scratch, explicit offset temporaries, and 180 historical AST permutations. Those permutations were
run before the current 96-97% structural gate and must not be repeated at the present score.

New source-hash-distinct axes measured after `57254d1`:

- repeated `entries[frame]` plus global-Y setup: 66.06%, reverted;
- volatile `gIcCnt`: 60.89% with a spill, reverted;
- collapsed or predeclared dim palette: no code change;
- semantic `gIcCnt2` read: 70.05% but an unwanted sixth `gIcCnt2` relocation, reverted;
- branch-duplicated `gIcCnt2` publication: optimized away;
- split dim pixel fetch: no code change;
- exact four-call literal form: 72.57%;
- function-wide unsigned `copyCount`: 69.46% with a spill, reverted;
- clipped-region unsigned `copyCount`: 73.60%, retained;
- signed, unclipped, and current-Y aliases: no code change, reverted.

New axes measured after `c164dcf`:

- literal clipped-first source order: 66.06%, reverted;
- explicit unclipped width cast: no code change;
- branch-assigned outer count: no code change;
- external-linkage `gIcCnt2`: no code change and wrong symbol identity, reverted;
- signed `gIcCnt2`: 73.5769%, reverted;
- external-linkage `gIcCnt`: no code change;
- initial common-tail do-block before operand reordering: 70.15385%;
- retail operand setup order in the common tail: 74.4533%, retained;
- clipped count moved after `right`: 73.7967%, reverted;
- clipped count moved before Y tests: 72.6951%, reverted;
- published `gIcRun` used as count: 72.6951%, reverted;
- sinkable right/count plus extended Y lifetime: 73.8654%, reverted;
- `currentY` alias alone: no code change.

Final axes measured after `4498053`:

- signed `cn`: 74.35714%, reverted;
- preinitialized then mutated `cn`: byte-identical to the checkpoint;
- external linkage for `gIcX0`/`gIcY`: byte-identical with wrong symbol identity;
- explicit `goto` dim-tail join: byte-identical;
- `gIcClipR` snapshot plus embedded `gIcCnt2` publication: byte-identical;
- retained final-assignment result plus a separate signed-test local: byte-identical;
- signed decoded `count`: 73.04%, reverted.

### Remaining concrete evidence

The known missing sites are setup `gIcX0`/`gIcY` CSE reloads and the fifth dim
`gIcCnt2` store. The literal quadrants now share the retail-style full-copy tail. Continue from
the remaining concrete setup/dim sites only after a shared header/compiler state changes or an
adjacent decoder reveals a new retail-proven lifetime. Do not repeat the axes above while the
canonical hash agrees. This is a scheduling defer, not proof that the residual is impossible.

## BASE/Iconf2b: FlipIconToBitmap

Status: clean soft defer pending a shared icon/header or compiler-state change. It is not an
accepted wall and must not receive `@early-stop`.

Canonical source state:

- checkpoint: `ea86fa8`
- target: RVA `0xd1ba0`, retail size `0x4f1`
- `src/BASE/Iconf2b.cpp`: `4db51d6a2e2bffb95cba2c36cb16fa7b0a690b6bd80e5ec60caed41fb50fa666`
- live checkpoint: 83.53%, 84 candidate vs 81 retail relocations, no base-only external target

### Corrections retained at the canonical checkpoint

- removed the invalid generic `@early-stop`;
- recovered early source cursor, width spill, and entry Y/X lifetimes;
- reordered clipped fill, dim, literal quadrants, and newline updates;
- corrected dim advancement to the original `gFlipCnt2` publication;
- used `gFlipDimLen` for the retail selected dim/literal publication;
- reconstructed the local dim cursor with a per-iteration global palette load;
- corrected the flipped right-edge boundary and shared literal loop;
- retained the assignment-expression decrement that removes one unwanted destination relocation.
- publish `gFlipClipR` before `gFlipClipB`, matching retail relocation order.

### Searches already exhausted

The removed pre-reconstruction wall had already tried local/global X bounds, early/late source
formation, volatile width/run, split/common clipped fills, local/global dim loops, offset
temporaries, and 180 historical AST permutations. Those AST passes predate the current structural
gate and must not be repeated below 96-97%.

A local dim-pointer form reached retail size `0x4f1` and 83 relocations but changed the frame to
four bytes and reduced instruction agreement; it was rejected.

New measured axes:

- combined extra boundary/count locals: 69.29%, reverted;
- isolated clipped destination local: 69.20%, reverted;
- separate width assignment: byte-identical;
- setup X/Y reuse: byte-identical;
- Y polarity/constant spellings: byte-identical;
- dim-count placement: byte-identical;
- address grouping: byte-identical;
- entry-publication placement: byte-identical;
- palette local outside the loop: 75.64% with 82 relocations, reverted because retail reloads the
  global palette per iteration;
- postfix `gFlipDst--`: byte-identical; the retained assignment expression removes one relocation.

Final measured axes after `ea86fa8`:

- mapped-byte dim initializer: 74.89% with 86 relocations, reverted;
- palette assignment through `px` after publication: 72.63% with 86 relocations, reverted;
- unsigned-int dim pixel index: byte-identical;
- volatile `gFlipClipB`: byte-identical;
- move `entryY` initialization after X subtraction: byte-identical;
- branch-local unsigned-char fill color: byte-identical.

Retest from the first structural/relocation divergence only after a real shared icon/header or
compiler-state change. Do not repeat these axes while the canonical hash agrees. A retained shared
change must also trigger the deferred Icon2b and icon2bc retests. This is a scheduling defer, not
proof that the remaining bytes are impossible.

## BASE/droplist: dropListWidget::ProcessSelectDialog

Status: clean soft defer pending a predecessor/shared-header compiler-state change. It is not an
accepted wall and has no `@early-stop`.

Canonical source state:

- checkpoint: `0cb12b3`
- target: RVA `0xdcb10`
- normalized function source hash: `40e2a77ca175`
- `src/BASE/droplist.cpp` SHA-256:
  `65610405f395c0f78bf14d329a5830f870bb32b4e8734e4d4862a7083a967514`
- live checkpoint: 99.36745%, raw COMDAT size 2079 bytes, 36 relocations
- `Read`, `DrawDropStuff`, `SaveDropBackground`, and `RestoreDropBackground` are exact;
  `Main` has its separate strict zero-unmasked-byte local-label proof

All external relocation offsets and targets agree. The local tables align at `+0x7d4/+0x7f8`;
their remaining identities are delinked local labels.

### Remaining true code residuals

- `+0x1b5`: candidate loads `[ebx+0x1c]` then adds `[ebx+0x18]`; retail loads
  `[ebx+0x18]` then adds `[ebx+0x1c]`;
- `+0x493`: candidate `cmp ecx,eax; jg` versus retail `cmp eax,ecx; jl`;
- `+0x792..+0x7a0`: candidate stores null before loading the Draw receiver/vtable; retail
  preloads vtable and `this`, stores null, then calls.

The formerly missing `mov ax,[ebx+0x3e]` reload at `+0x368` is solved and integrated: the
decrement and second guarded top-index update must be separate statements.

### Searches already exhausted

Commutative geometry/load forms, all byte-identical to the checkpoint:

- `m_x + m_width - scrollWidth[0]`;
- `m_width + m_x - scrollWidth[0]`;
- `static_cast<short>(m_x + m_width) - scrollWidth[0]`;
- `m_x - scrollWidth[0] + m_width`;
- `m_width - scrollWidth[0] + m_x`.

Compare forms at `+0x493`, all byte-identical:

- `if (item >= field_0x32)`;
- `if (field_0x32 <= item)`;
- ternary clamp `item < field_0x32 ? item : field_0x32 - 1`;
- positive empty-arm plus `else`.

Cleanup/Draw scheduling forms, all byte-identical unless noted:

- baseline null assignment then Draw;
- truthy cleanup guard; explicit `this->Draw()`; comma statement;
- assignment folded into true `if`, false/`else`, or conditional operator;
- `(m_savedBackground = 0, this)->Draw()`: 95.97%, rejected;
- inlining real Restore: same Process bytes but suppresses the exact Restore symbol, rejected;
- paired inline Save/Restore: 95.99% and suppresses exact helpers, rejected;
- stale bare-delete retest, file SHA
  `60a0c22ddad13261eefc56f19234b6020a5498545fd61f8ea80c0bcc4341d2cc`:
  98.06208%, raw 2075, rejected.

Libclang search on the current reload CFG pinned Process and all 13 siblings:

- ctor 3 variants; dtor 2; Read 27; DeleteItem 11; strict Main 37;
- Restore 1; DrawDropStuff 51; Save had no legal mutation;
- Process itself had 116 legal variants plus a bounded 100-step walk.

No variant improved Process. Unsafe inequality +/-1 was disabled, the temporary tool state was
restored, and the regex permuter was never used.

Exact non-AST predecessor states, all rejected/restored:

- Draw top-only ternary, SHA
  `1b12bfab6008436a81ab202b36ad45aa9c77f7c1c50f4993509460424dfeea07`:
  Draw exact, Process 93.419464/raw 2083;
- Draw bottom-only, SHA
  `d8d5fc262170ed466d09ba72f70f965e9090aadf0d669d25c178848dbc367da4`:
  same result;
- paired Draw ternaries, SHA
  `d28fafcdbf35b0774e11d686e2a2e3fd326224ebb051566fc55a3bb7e578eb77`:
  Draw exact, Process 96.38423/raw 2087;
- Restore bare delete, SHA
  `8a50df5d8fbd04b813ae95871a399b6e1a50d7787529253ab5009d68cfabca4a`:
  Restore exact, Process 95.156044/raw 2075;
- Save combined assignment/call, SHA
  `86ec5679c5afbe16d17a8a2d378ef48ae22a24e36630eb69abbb19737c22fb73`:
  Save exact, Process unchanged;
- Draw split frame locals, SHA
  `e5b974ed113ba24e83afe00f94ea0689c90f2f932ffa1265b55a3bb7e578eb77`:
  Draw exact, Process 96.40269/raw 2087.

Older closed axes: selected getter value/reference; scalar, pointer, and wider-scope selected
snapshots; prefix/compound decrement; nested block, do/while, and short-circuit forms; helper
inline calls; nonvirtual declaration order; local renames; the obsolete `maxItems` local; and
already-integrated body order/polarity. Do not repeat them without a material compiler-state
change.

### Retest trigger

Retest only when the normalized Process source hash changes from `40e2a77ca175`, a predecessor
source hash changes, or an intentional shared declaration/layout edit changes droplist raw bytes.
Relevant headers include `dropListWidget.h`, `widget.h`, `bitmap.h`, `icon.h`, `font.h`,
`inputManager.h`, `mouseManager.h`, and `windowManager.h`.

On a trigger, rebuild and raw-compare the three exact residual offsets first. Re-run only the
legal AST/predecessor matrix associated with an offset that changed. The current state is a
scheduling defer, not proof that the residual is impossible.
