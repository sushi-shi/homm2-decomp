/*
IconToBitmap, RVA 0x000d0570, retail size 1261.
Serialized-field ordering and coordinate-publication campaign, 2026-07-27.

Retail computes the 12-byte frame offset, reads entry X and srcOffset through
that offset, and only then binds the IconEntry pointer.  The previous member-
accessor source bound the pointer first.  Reconstructing the retail order:

    i32 entryOffset = frame * sizeof(IconEntry);
    i32 entryX =
        reinterpret_cast<IconEntry*>(data + entryOffset)->x;
    i32 sourceOffset =
        reinterpret_cast<IconEntry*>(data + entryOffset)->srcOffset;
    IconEntry* entry =
        reinterpret_cast<IconEntry*>(data + entryOffset);

improved clean matching from 73.469780% to 75.241760%.  Its complete clean plus
50 forest-state sweep reached 81.230770%, size 1219, 79/83 relocations, and
exact 80-block flow.  This exceeded the prior 80.890110% MAX.

Four coordinate-publication structures were then crossed independently with
the clean state and all 50 forest states (204/204 candidates):

  - staged X/Y/pitch locals followed by publication,
  - assignment-result publication,
  - mutating the x/y formals before publication, following the flip sibling,
  - publishing globals first and snapshotting them afterward.

The formal-mutation family reached 81.494510% in six states, including trial
8.  It emits 1220 bytes, 79/83 relocations, exact 80-block flow, and improves
the block partition from 62 exact plus 18 size-only to 63 exact plus 17
size-only.  Its clean state is also stronger at 79.318680%.  This family is
retained.

Additional complete clean-plus-50 descendants:

  - an inline X-origin helper peaked at 81.203300%, size 1220, 79/83;
  - const entry snapshots were byte-identical to the direct declarations;
  - split declaration/assignment peaked at 81.230770% but was one byte longer;
  - the retail-observed left-clipped gIcCnt2 republication, with and without a
    clip-right snapshot, was folded in every state and reproduced 81.230770%.

Artifacts:
  build/match-variants/icon2b-current-member-entry-states-20260727/results.json
  build/match-variants/icon2b-fields-before-entry-states-20260727/results.json
  build/match-variants/icon2b-fields-x-helper-states-20260727/results.json
  build/match-variants/icon2b-fields-const-states-20260727/results.json
  build/match-variants/icon2b-fields-split-init-states-20260727/results.json
  build/match-variants/icon2b-fields-left-count-states-20260727/results.json
  build/match-variants/icon2b-fields-left-clipright-count-states-20260727/results.json
  build/match-variants/icon2b-fields-publication-states-20260727/results.json

No exact closure appeared.  The best retained island remains 41 bytes short
and lacks one ordered occurrence each of gIcCnt2, gIcPitch, gIcX0, and gIcY.
*/

#if 0
// Retained coordinate-publication structure.
x += entryX;
y += entry->y;
i32 pitch = dest->m_width;
gIcX0 = x;
gIcPitch = pitch;
gIcY = y;
i32 X = x;
i32 currentY = y;
#endif
