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

Status: active. This section records superseded axes while the same lane continues; it is not a
wall or soft defer.

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

Continue from the first remaining structural/relocation divergence. Do not repeat these axes while
the canonical hash agrees. If a real shared icon/header change is retained, retest the deferred
Icon2b and icon2bc functions before final handoff.

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
