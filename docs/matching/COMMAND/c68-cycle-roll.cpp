// VC6 SP5 COMMAND source family c68-cycle-roll; parent34b6e303 plus U17 packet view.
// Source record only. All alternatives below are relative to the stated BEFORE,
// not cumulative diffs. No cross-family/TU-state census is claimed.
// build/source-variant-batch/c68-cycle-roll/results.json; RVA 0x315fa
// Complete 2 arms; elapsed 2.306106s; input restored.
// Disposition: retain all removals (row_index for A03); combined all98 native proof.
// EH private symbol hashes may differ; native comparison canonicalizes actual section owners.
// trial / choices / bytes / text hash / normalized reloc hash / blocks / exact,size,shift,flow / graph exact
// 0 / {'float_denominator': 'keep', 'candidate': 'baseline'} / 2352 / 76e1a8f3b42919dd / f11b94a8bd923c04 / 108:108 / 108,0,0,0 / True
// 1 / {'float_denominator': 'remove', 'candidate': 'baseline'} / 2352 / 76e1a8f3b42919dd / f11b94a8bd923c04 / 108:108 / 108,0,0,0 / True

// AXIS float_denominator
// BEFORE (complete exact span):
// | static_cast<float>(Random(IDLE_ROLL_MIN, IDLE_ROLL_MAX))
// ALTERNATIVE remove (unified source delta from BEFORE):
// | --- before
// | +++ remove
// | @@ -1 +1 @@
// | -static_cast<float>(Random(IDLE_ROLL_MIN, IDLE_ROLL_MAX))
// | +Random(IDLE_ROLL_MIN, IDLE_ROLL_MAX)
