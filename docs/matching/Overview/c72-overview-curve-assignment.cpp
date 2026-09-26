// VC6 SP5 Overview source family c72-overview-curve-assignment; parent ee5ee1cf plus retained C72/S65, formatting and seven restored type-boundary casts.
// Source record only. All alternatives below are relative to the stated BEFORE,
// not cumulative diffs. No cross-family/TU-state census is claimed.
// build/source-variant-batch/c72-overview-curve-assignment/results.json; RVA 0x79ba0
// Complete 2 arms; elapsed 1.683706s; input restored.
// Disposition: retain this one numeric-cast removal; combined all98 native proof.
// EH private symbol hashes may differ; native comparison canonicalizes actual section owners.
// trial / choices / bytes / text hash / normalized reloc hash / blocks / exact,size,shift,flow / graph exact
// 0 / {'numeric0': 'keep', 'candidate': 'baseline'} / 7750 / 628178f0823444cb / ed396a83196242e5 / 234:234 / 233,1,0,0 / True
// 1 / {'numeric0': 'implicit', 'candidate': 'baseline'} / 7750 / 628178f0823444cb / 60d43e1fc2fe144d / 234:234 / 233,1,0,0 / True

// AXIS numeric0
// BEFORE (complete exact span):
// | static_cast<i16>(
// |                 giOverviewTop[IDX(giOverviewType)] * pixelsPerItem + OVERVIEW_SCROLL_MIN_Y_FLOAT
// |                 + OVERVIEW_SCROLL_ROUNDING_OFFSET
// |             )
// ALTERNATIVE implicit (unified source delta from BEFORE):
// | --- before
// | +++ implicit
// | @@ -1,4 +1,2 @@
// | -static_cast<i16>(
// | -                giOverviewTop[IDX(giOverviewType)] * pixelsPerItem + OVERVIEW_SCROLL_MIN_Y_FLOAT
// | -                + OVERVIEW_SCROLL_ROUNDING_OFFSET
// | -            )
// | +(giOverviewTop[IDX(giOverviewType)] * pixelsPerItem + OVERVIEW_SCROLL_MIN_Y_FLOAT
// | +                + OVERVIEW_SCROLL_ROUNDING_OFFSET)
