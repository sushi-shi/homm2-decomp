// VC6 SP5 Overview source family s65-overview-mine-array; parent ee5ee1cf.
// Source record only. All alternatives below are relative to the stated BEFORE,
// not cumulative diffs. No cross-family/TU-state census is claimed.
// build/source-variant-batch/s65-overview-mine-array/results.json; RVA 0x7bda1
// Complete 2 arms; elapsed 1.675519s; input restored.
// Disposition: retain the seven-element mine array; combined all98 native proof.
// EH private symbol hashes may differ; native comparison canonicalizes actual section owners.
// trial / choices / bytes / text hash / normalized reloc hash / blocks / exact,size,shift,flow / graph exact
// 0 / {'source_family': 'keep', 'candidate': 'baseline'} / 1202 / 688689bd5599e522 / 9cebe67a573ac9ef / 42:42 / 42,0,0,0 / True
// 1 / {'source_family': 'seven_elements', 'candidate': 'baseline'} / 1202 / 688689bd5599e522 / 9cebe67a573ac9ef / 42:42 / 42,0,0,0 / True

// AXIS source_family
// BEFORE (complete exact span):
// |     OVERVIEW_TITLE_COUNT           = 3,
// |     OVERVIEW_MINE_COUNT_CAPACITY   = IDX(RES_COUNT) + 1,
// |     OVERVIEW_TROOP_TEXT_CAPACITY   = 6,
// ALTERNATIVE seven_elements (unified source delta from BEFORE):
// | --- before
// | +++ seven_elements
// | @@ -1,3 +1,3 @@
// |      OVERVIEW_TITLE_COUNT           = 3,
// | -    OVERVIEW_MINE_COUNT_CAPACITY   = IDX(RES_COUNT) + 1,
// | +    OVERVIEW_MINE_COUNT_CAPACITY   = IDX(RES_COUNT),
// |      OVERVIEW_TROOP_TEXT_CAPACITY   = 6,
