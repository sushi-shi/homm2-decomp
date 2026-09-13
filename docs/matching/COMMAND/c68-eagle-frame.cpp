// VC6 SP5 COMMAND source family c68-eagle-frame; parent34b6e303 plus U17 packet view.
// Source record only. All alternatives below are relative to the stated BEFORE,
// not cumulative diffs. No cross-family/TU-state census is claimed.
// build/source-variant-batch/c68-eagle-frame/results.json; RVA 0x2e93a
// Complete 2 arms; elapsed 2.273207s; input restored.
// Disposition: retain all removals (row_index for A03); combined all98 native proof.
// EH private symbol hashes may differ; native comparison canonicalizes actual section owners.
// trial / choices / bytes / text hash / normalized reloc hash / blocks / exact,size,shift,flow / graph exact
// 0 / {'byte_to_frame': 'keep', 'candidate': 'baseline'} / 754 / ec760cf61e682d6c / 9c5789f8f2a78129 / 16:16 / 16,0,0,0 / True
// 1 / {'byte_to_frame': 'remove', 'candidate': 'baseline'} / 754 / ec760cf61e682d6c / 21d080e0e64518eb / 16:16 / 16,0,0,0 / True

// AXIS byte_to_frame
// BEFORE (complete exact span):
// | static_cast<i16>(gsSpellInfo[IDX(newSpell)].iconIndex)
// ALTERNATIVE remove (unified source delta from BEFORE):
// | --- before
// | +++ remove
// | @@ -1 +1 @@
// | -static_cast<i16>(gsSpellInfo[IDX(newSpell)].iconIndex)
// | +gsSpellInfo[IDX(newSpell)].iconIndex
