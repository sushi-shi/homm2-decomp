// VC6 SP5 Overview source family c72-overview-knob-float; parent ee5ee1cf.
// Source record only. All alternatives below are relative to the stated BEFORE,
// not cumulative diffs. No cross-family/TU-state census is claimed.
// build/source-variant-batch/c72-overview-knob-float/results.json; RVA 0x7c253
// Complete 4 arms; elapsed 2.851027s; input restored.
// Disposition: retain this family's cast removals; combined all98 native proof.
// EH private symbol hashes may differ; native comparison canonicalizes actual section owners.
// trial / choices / bytes / text hash / normalized reloc hash / blocks / exact,size,shift,flow / graph exact
// 0 / {'source_family': 'keep', 'candidate': 'baseline'} / 686 / bca9984e0fd80ef9 / a3aa3dc9463b4ea9 / 27:27 / 27,0,0,0 / True
// 1 / {'source_family': 'assign1', 'candidate': 'baseline'} / 686 / bca9984e0fd80ef9 / a3aa3dc9463b4ea9 / 27:27 / 27,0,0,0 / True
// 2 / {'source_family': 'assign0', 'candidate': 'baseline'} / 686 / bca9984e0fd80ef9 / a3aa3dc9463b4ea9 / 27:27 / 27,0,0,0 / True
// 3 / {'source_family': 'assign0_assign1', 'candidate': 'baseline'} / 686 / bca9984e0fd80ef9 / a3aa3dc9463b4ea9 / 27:27 / 27,0,0,0 / True

// AXIS source_family
// BEFORE (complete exact span):
// |     {
// |         scrollRange = static_cast<float>(SCROLL_RANGE);
// |         topValue = static_cast<float>(SCROLL_MIN_Y);
// |         topBefore = giOverviewTop[IDX(giOverviewType)];
// ALTERNATIVE assign1 (unified source delta from BEFORE):
// | --- before
// | +++ assign1
// | @@ -1,4 +1,4 @@
// |      {
// |          scrollRange = static_cast<float>(SCROLL_RANGE);
// | -        topValue = static_cast<float>(SCROLL_MIN_Y);
// | +        topValue = SCROLL_MIN_Y;
// |          topBefore = giOverviewTop[IDX(giOverviewType)];
// ALTERNATIVE assign0 (unified source delta from BEFORE):
// | --- before
// | +++ assign0
// | @@ -1,4 +1,4 @@
// |      {
// | -        scrollRange = static_cast<float>(SCROLL_RANGE);
// | +        scrollRange = SCROLL_RANGE;
// |          topValue = static_cast<float>(SCROLL_MIN_Y);
// |          topBefore = giOverviewTop[IDX(giOverviewType)];
// ALTERNATIVE assign0_assign1 (unified source delta from BEFORE):
// | --- before
// | +++ assign0_assign1
// | @@ -1,4 +1,4 @@
// |      {
// | -        scrollRange = static_cast<float>(SCROLL_RANGE);
// | -        topValue = static_cast<float>(SCROLL_MIN_Y);
// | +        scrollRange = SCROLL_RANGE;
// | +        topValue = SCROLL_MIN_Y;
// |          topBefore = giOverviewTop[IDX(giOverviewType)];
