# Icon-family semantic structure recovery plan

The shared RLE macro experiment is retired as matching source. Its useful
result was a vocabulary of semantic decoder blocks. Recover those blocks as
explicit C++ in every TU, with `IconToBitmapYModify` as the strongest known
structural reference.

This is a whole-tree investigation. A conversion is not accepted because the
first divergent bytes improve. Every candidate is compared across its complete
CFG and across compiler-state islands.

## Acceptance gates

For each source shape, keep byte and topology objectives independent:

1. Compile the clean source.
2. Compare the complete block skeleton with
   `homm2 sema disasm <rva> --blocks --diff --lite`.
3. Inspect the full graph with `--blocks --diff --dot` when labels, joins, or
   predecessor ownership differ.
4. Record block count, labeled-edge delta, predecessor delta, flow-kind delta,
   target shift, and size-only blocks. Do not stop at the first divergent
   block.
5. Cross every reviewed structural variant with the clean TU state and a
   bounded 50-state island census. Source shape and compiler state are
   independent Cartesian axes.
6. Retain both the best-topology and best-fuzzy artifacts when they differ.
7. Audit retail size and ordered relocations before exact closure. Never lower
   a retained historical MAX merely because the current clean object moved.

A lower-scoring island with the correct whole graph remains a useful parent
for further work. A higher fuzzy score with the wrong graph is not structural
progress.

## Semantic blocks to recover

### 1. File-owned state

Keep each decoder's scratch storage at file scope and declare it in the
semantic order established by the historical exact `IconToBitmapYModify`
source. Function-local static ownership was exhaustively rejected for that
reference even though its CFG remained exact. Do not restore state-declaration
macros and do not share scratch owners across TUs.

- general ownership: `s_entry`, `s_src`;
- output geometry: `s_left`, `s_right`, `s_x`, `s_y`, `s_pitch`, `s_row`;
- clipping: `s_clipR`, `s_clipB`;
- command dispatch: `s_run`;
- solid fill: `s_color`;
- dim/recolor: `s_dimPal`, `s_dimLen`, `s_dimDst`;
- literal spans: `s_literalSource`, `s_srcSkip`;
- published work state: `s_spanCount`, `s_loopCount` or `s_loopIndex`, `s_dst`.

Each function should visibly acquire only the groups needed by its decoder
dialect. `DATA(...)` addresses remain source evidence; declaration order is a
separate compiler-state axis, not a reason to obscure the semantic groups.

Moving storage scope changes private COFF identities even when names, types,
addresses, and accesses are unchanged. Compile the raw candidate first, then
run `homm2 redelink`, then `homm2 build`; regenerating topology from the stale
pre-move object produces misleading normalization warnings.

### 2. Entry selection and source publication

Use one ownership pattern throughout the family:

```cpp
IconEntry* entries = srcIcon->Entries();
s_entry = &entries[frame];
s_src = srcIcon->m_data + s_entry->srcOffset;
```

Then derive origin and row state from `s_entry`. Remove arbitrary mixtures of
manual `sizeof(IconEntry)` byte offsets, repeated `entries[frame]` expressions,
setup-only `entryX`/`entryY`/`sourceOffset` locals, and local `entry` aliases.
Reverse decoders retain their different horizontal arithmetic, but entry
selection and cursor publication should have the same semantic ownership.

Test the publication order as a structural axis; do not assume its clean
spelling selects the correct compiler island.

### 3. Origin, clipping, and initial row

Keep these as three visible setup phases in every TU:

1. derive `s_left`/`s_right`, `s_x`, pitch, and `s_y`;
2. normalize clip mode and publish `s_clipR`/`s_clipB`;
3. construct the initial output row.

Use narrow TU-local inline helpers only where the helper owns one complete
operation:

- `NeedsClipping(entry, left, y, clipX, clipY, clipW, clipH)`;
- `RowVisible(clipTop, y, clipBottom)`;
- the sheared form of row visibility where the shear lookup is part of the
  retail expression;
- `InitialRow(dest, pitch, y)` only where inline evidence supports it.

Forward and reverse helpers should use the same parameter order and semantic
names. They may have different bodies when retail behavior is genuinely
asymmetric. In particular, preserve the shipped reverse clipping defect rather
than “repairing” its unreachable clamp arm.

### 4. Command-loop skeleton

Every explicit decoder should read as the same sequence of phases:

1. command fetch and source-cursor publication;
2. signed command classification;
3. skip-or-end;
4. solid or dim dispatch;
5. positive literal/span handling;
6. zero/new-row handling;
7. loop continuation.

Test command fetch as a source axis:

- `ReadIconRleByte(s_src)`;
- explicit read followed by cursor increment, matching exact YModify;
- a local cursor with publication at the same semantic sites.

The selected form must be consistent within a dialect and justified by the
whole CFG, stack slots, and relocation sites.

### 5. Skip-or-end ownership

Keep publication of `s_x`, `s_row`, `s_src`, and `s_run` at the same semantic
site across siblings. The command mask and cursor direction are dialect
parameters, not different conceptual handlers.

The mono reverse decoders must publish `s_x` at the loop head on every
iteration, as retail does. Do not let a helper move that store into the skip
arm.

### 6. Solid and dim joins

Recover the explicit label topology visible in the exact reference:

- decode a solid command;
- decode a dim/recolor command;
- join only where both genuinely share the same fill operation;
- keep dim pixel transformation in its own semantic phase.

Test `fill_run`/`do_fill` and `dim_run`/`do_dim` placement as reviewed
structural variants. A label is valuable when it expresses a real many-to-one
join; it is not retained merely to perturb code generation.

Narrow pixel helpers may own exactly one transform:

- identity/copy;
- color-table lookup;
- dim-palette lookup;
- conditional ColorTable dimming.

They must not own span clipping, counter lifetime, cursor publication, or
control transfer.

### 7. Literal-span ownership

Keep the literal source, destination, count, clipping geometry, and pixel loop
visible together. Test these alternatives without combining them prematurely:

- direct source cursor versus a published `s_literalSource`;
- ascending versus descending destination walk;
- copied versus color-mapped pixel operation;
- complete nested clipping branches versus precomputed pointer/count tuples.

The exact YModify evidence favors a narrow clipped-source expression helper,
not a generic literal-handler abstraction.

### 8. Row transition

The loop tail owns row advancement, source publication, and reconstruction of
the horizontal origin. Forward, reverse, mono, and sheared decoders differ in
the arithmetic, but should expose the same semantic phase boundary.

Test whether the new-row block rejoins the loop head directly or through a
continuation label. Compare every predecessor and outgoing edge, not only its
instruction count.

## Family order

Recover structure in related pairs so evidence transfers without assuming
identical text:

1. `IconToBitmapYModify` / `FlipIconToBitmapYModify`;
2. `IconToBitmap` / `IconToBitmapColorTable`;
3. `FlipIconToBitmap` / `FlipIconToBitmapColorTable`;
4. `DimIconToBitmap` / `FlipDimIconToBitmap`;
5. `MonoIconToBitmap` / `FlipMonoIconToBitmap`.

For each pair, first align entry/setup ownership, then helper roles, then the
command skeleton, then individual handlers. After every accepted structural
step, run the whole-tree comparison and its 50-state census before stacking
the next step.

## Attempt records

Store each reviewed source shape under
`docs/matching/<function-name>/<attempt-name>.cpp`. Its header should record:

- the semantic axis changed;
- the clean effective-source hash;
- clean fuzzy score and retail/candidate sizes;
- complete topology metrics;
- the 50-state best-topology and best-fuzzy results;
- relocation result;
- why the shape was retained, rejected, or kept only as a parent island.

Generated probe source is disposable and must not enter reconstructed source.
Generated baseline/config files are refreshed only through their scripts.
