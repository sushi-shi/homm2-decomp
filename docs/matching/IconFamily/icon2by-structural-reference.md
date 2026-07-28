# `IconToBitmapYModify` structural reference

The historical exact `IconToBitmapYModify` source is the strongest structural
reference in the icon-decoder family. Effective-source hash
`15fda1250f72.852e9521ff86` reached audited exact closure with retail size 1416
and all 130 ordered relocations. That exact source is preserved in
`docs/matching/IconToBitmapYModify/exact-structure-before-semantic-renames.cpp`.

Later semantic transformations changed the effective source hash. After the
2026-07-28 family rebuild, the current hash is
`05f27a372010.30f5cab9fe5a` and its current hash-scoped MAX is 92.9569%.
The old 100% is historical evidence only; it must not appear as the live
hash's MAX. Use the archived exact source as the structural reference and
reproduce evidence from the current object before transferring another detail.

## Transferable source structure

The function reads as one explicit decoder rather than a composition of shared
handlers:

1. Select the frame entry and publish the entry, source cursor, sprite origin,
   pitch, row, and clip bounds.
2. Enter one `for (;;)` command loop.
3. Read and advance the source cursor as two explicit statements.
4. Dispatch a negative command into skip/end, solid, or dim handling.
5. Route solid and recolor-dim commands through the same `do_fill` label.
6. Keep dim decoding and its pixel loop in the negative-command arm.
7. Handle a positive literal command after the negative-command arm.
8. Treat zero as the row transition and rebuild X from the row shear plus the
   original left edge.

Within each span handler, clipping geometry is expressed as complete nested
branches. The fill and literal bodies do not first reduce their behavior to a
generic selected pointer/count tuple. Cursor publication, counter ownership,
and pointer advancement remain visible at their semantic sites.

The only inline boundaries in the exact source are narrow operations:

- `IconRowVisible(shear, clipTop, currentY, clipBottom)` owns the repeated
  shear and vertical-clip predicate without depending on file state by name.
- `IconOutsideCopySource` owns one clipped literal-source expression.

These boundaries are evidence that a small repeated expression may have been
an original inline function. They are not evidence for a shared decoder macro.

Semantic `s_*` scratch names and the parameterized helper each reached audited
100% closure in intermediate source hashes. Moving the scratch declarations
into function scope did not: 200 state probes and a 306-candidate
declaration-order product kept the exact CFG but found no exact
byte/relocation island. File-scope scratch ownership is therefore part of the
historical retail structure evidence.

## Dialect differences that must remain explicit

The siblings cannot copy the function text mechanically:

- ordinary forward decoders have clipped and unclipped paths;
- ColorTable decoders map solid/literal pixels and optionally gate dimming;
- reverse decoders walk spans and row origins in the opposite direction;
- mono decoders use their separate command mask and no literal-pixel payload;
- YModify adds a per-row shear and always applies its supplied clip rectangle.

These differences should remain ordinary C++ in each TU. The reusable evidence
is the ordering and ownership of semantic blocks, not identical source text.

## Propagation campaign

Test the reference structure through source variants instead of editing the
siblings one small statement at a time. Keep byte fuzzy and canonical CFG
topology as independent objectives.

Structural consistency is the primary acceptance gate for this family. A
candidate should keep the same semantic phase boundaries, control-flow joins,
cursor/counter ownership, and narrow inline-helper roles as the reference.
Helpers should use consistent names, parameter order, and responsibility across
siblings; only genuine forward/reverse, mono, ColorTable, dim, and shear
dialect differences should vary. A higher fuzzy score does not justify
retaining an inconsistent spelling.

For each sibling, cross these axes independently before combining survivors:

1. setup/publication order matching the reference;
2. explicit byte-read followed by cursor advance;
3. skip/end publication and continuation ownership;
4. shared solid/recolor `do_fill` label;
5. inline dim decode and pixel-loop ownership;
6. literal clipping expressed as complete nested branches;
7. row-transition ownership at the loop tail.

Start with `IconToBitmap` and `IconToBitmapColorTable`, then transfer surviving
mechanisms to the reverse, mono, dim-only, and flipped-Y dialects. A lower fuzzy
island with a more faithful canonical graph remains a valid structural
descendant and should be retained as an artifact for further permutations.
