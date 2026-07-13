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

Status: active; this section records superseded axes while the same lane continues. It is not a
soft defer or an accepted wall.

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

### Remaining concrete evidence

The known missing sites are setup `gIcX0`/`gIcY` CSE reloads and the fifth dim
`gIcCnt2` store. The literal quadrants now share the retail-style full-copy tail. Continue from
the remaining concrete setup/dim sites; do not repeat the axes above unless a shared
header/compiler state changes.
