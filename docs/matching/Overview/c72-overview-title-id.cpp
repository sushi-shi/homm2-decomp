// VC6 SP5 Overview source family c72-overview-title-id; parent ee5ee1cf.
// Source record only. All alternatives below are relative to the stated BEFORE,
// not cumulative diffs. No cross-family/TU-state census is claimed.
// build/source-variant-batch/c72-overview-title-id/results.json; RVA 0x7b9e6
// Complete 2 arms; elapsed 1.679242s; input restored.
// Disposition: retain this family's cast removals; combined all98 native proof.
// EH private symbol hashes may differ; native comparison canonicalizes actual section owners.
// trial / choices / bytes / text hash / normalized reloc hash / blocks / exact,size,shift,flow / graph exact
// 0 / {'source_family': 'keep', 'candidate': 'baseline'} / 836 / a25918932a730b77 / 8d7b7c4cb4c90edd / 26:26 / 26,0,0,0 / True
// 1 / {'source_family': 'title_id', 'candidate': 'baseline'} / 836 / a25918932a730b77 / ddf166494d2db427 / 26:26 / 26,0,0,0 / True

// AXIS source_family
// BEFORE (complete exact span):
// |             FONT_DRAW_DEFAULT,
// |             static_cast<i16>(col + OVERVIEW_COLUMN_TITLE_FIRST),
// |             WIDGET_KIND_TEXT,
// ALTERNATIVE title_id (unified source delta from BEFORE):
// | --- before
// | +++ title_id
// | @@ -1,3 +1,3 @@
// |              FONT_DRAW_DEFAULT,
// | -            static_cast<i16>(col + OVERVIEW_COLUMN_TITLE_FIRST),
// | +            col + OVERVIEW_COLUMN_TITLE_FIRST,
// |              WIDGET_KIND_TEXT,
