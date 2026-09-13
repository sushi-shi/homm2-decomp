// VC6 SP5 Overview source family s65-overview-unused; parent ee5ee1cf.
// Source record only. All alternatives below are relative to the stated BEFORE,
// not cumulative diffs. No cross-family/TU-state census is claimed.
// build/source-variant-batch/s65-overview-unused/results.json; RVA 0x79ba0
// Complete 2 arms; elapsed 1.73161s; input restored.
// Disposition: retain original; every tested alternative changes text.
// EH private symbol hashes may differ; native comparison canonicalizes actual section owners.
// trial / choices / bytes / text hash / normalized reloc hash / blocks / exact,size,shift,flow / graph exact
// 0 / {'source_family': 'keep', 'candidate': 'baseline'} / 7750 / 628178f0823444cb / 961908a478c1be46 / 234:234 / 233,1,0,0 / True
// 1 / {'source_family': 'unused', 'candidate': 'baseline'} / 7744 / 257181beb2a44a1d / bd8fa6f80d436398 / 234:234 / 233,1,0,0 / True

// AXIS source_family
// BEFORE (complete exact span):
// |         } else {
// |             i32 H2_UNUSED(unused);
// |             i32 steps = giOverviewItems[IDX(giOverviewType)] - OVERVIEW_VISIBLE_ROWS;
// ALTERNATIVE unused (unified source delta from BEFORE):
// | --- before
// | +++ unused
// | @@ -1,3 +1,2 @@
// |          } else {
// | -            i32 H2_UNUSED(unused);
// |              i32 steps = giOverviewItems[IDX(giOverviewType)] - OVERVIEW_VISIBLE_ROWS;
