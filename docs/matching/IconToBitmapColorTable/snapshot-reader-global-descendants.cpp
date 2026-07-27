/*
IconToBitmapColorTable (RVA 0x000d32a0, retail size 1455)
Snapshot/reader/global-publication descendants, 2026-07-27.

This experiment used the current effective source hash f0bc3ae5885f. The live
object is 82.154930%, size 1428, with 89/91 raw comparison relocations. Its
retained hash-scoped MAX is 82.8122%.

Family and block evidence:

  - IconToBitmap and the Y-modified family use ReadIconRleByte at the command
    boundary, while the retained color-table source manually advances gCTSrc.
  - The target and candidate both have 86 blocks and exact flow.
  - The first block-size divergence is B2 (candidate seven instructions,
    retail eight): retail reloads the published gCTX0/entry-width ownership.
  - B4, B7, and B9 then expose Y-publication and zeroing/materialization drift.
  - The previously retained snapshot-before-publication clue is five
    relocations short: one gCTX0, one gCTCnt, one gCTSrc, and two gCTY.

The first complete matrix layered the family reader boundary over both Y
lifetimes:

  published gCTY, manual reader:
    82.776990%, size 1428, 89/91 relocs, 69 exact + 17 size-only blocks

  published gCTY, inline reader:
    82.286385%, size 1438, 90/91 relocs, 67 exact + 19 size-only blocks

  snapshot Y then publish, manual reader:
    83.645540%, size 1410, 86/91 relocs, 69 exact + 17 size-only blocks

  snapshot Y then publish, inline reader:
    83.154930%, size 1420, 87/91 relocs, 67 exact + 19 size-only blocks

All four shapes were crossed with clean plus 50 expansive declaration-forest
states. The complete 204/204 run took 157.84 seconds, produced 74 normalized
states, was not truncated, and restored source. The inline reader recovers the
missing gCTSrc relocation and ten bytes, but does not combine the snapshot
byte island with the reader's block-body orbit.

The second complete matrix fixed the snapshot-plus-reader seed and crossed
independent X and Y predicate ownership:

  local or published X, snapshot Y:
    83.154930%, size 1420, 87/91 relocs, 67 exact + 19 size-only blocks

  local or published X, published Y:
    82.286385%, size 1438, 90/91 relocs, 67 exact + 19 size-only blocks

All four descendants were again crossed with clean plus 50 forest states.
The complete 204/204 run took 158.03 seconds, produced 37 normalized states,
was not truncated, and restored source. Published-X was byte-neutral.
Published-Y restores three relocation sites and approaches retail size, but
collapses exactly onto the already-known published-Y/reader orbit rather than
forming a hybrid with the snapshot byte island. The remaining missing
relocation is gCTCnt.

Artifacts:
  build/match-variants/icon2bc-snapshot-reader-layer-axes-20260727.json
  build/match-variants/icon2bc-snapshot-reader-layer-clean-20260727.json
  build/match-variants/icon2bc-snapshot-reader-layer-clean-islands-20260727/results.json
  build/match-variants/icon2bc-snapshot-reader-layer-states-20260727.json
  build/match-variants/icon2bc-snapshot-reader-layer-state-islands-20260727/results.json
  build/match-variants/icon2bc-snapshot-reader-global-descendants-axes-20260727.json
  build/match-variants/icon2bc-snapshot-reader-global-descendants-clean-20260727.json
  build/match-variants/icon2bc-snapshot-reader-global-descendants-clean-islands-20260727/results.json
  build/match-variants/icon2bc-snapshot-reader-global-descendants-states-20260727.json
  build/match-variants/icon2bc-snapshot-reader-global-descendants-state-islands-20260727/results.json

Disposition: retain the current reconstructed source and official 82.8122%
MAX. The higher snapshot results belong to different effective source hashes,
remain four or five ordered relocations short, and contradict retail's later
global reload topology. The unchanged-source branch did not exceed its
official MAX, so no status update is justified.
*/

#if 0
// Snapshot-before-publication seed.
i32 currentY = entry->y + y;
gCTY = currentY;

// Family reader boundary; applied to all five initial RLE reads.
cmd = ReadIconRleByte(gCTSrc);
count = ReadIconRleByte(gCTSrc);
gCTColor = colorTable[ReadIconRleByte(gCTSrc)];
flags = ReadIconRleByte(gCTSrc);

// Independent initial-predicate publication descendants.
if (gCTX0 < clipX || clipW + clipX < entry->w + gCTX0 || gCTY < clipY
    || clipY + clipH < entry->h + gCTY) {
    // Existing clipping body.
}
#endif
