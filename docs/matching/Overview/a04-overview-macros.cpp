// VC6 SP5 Overview source family a04-overview-macros; parent ee5ee1cf.
// Source record only. All alternatives below are relative to the stated BEFORE,
// not cumulative diffs. No cross-family/TU-state census is claimed.
// build/source-variant-batch/a04-overview-macros/results.json; RVA 0x79ba0
// Complete 2 arms; elapsed 1.933567s; input restored.
// Disposition: retain original; every tested alternative changes text.
// EH private symbol hashes may differ; native comparison canonicalizes actual section owners.
// trial / choices / bytes / text hash / normalized reloc hash / blocks / exact,size,shift,flow / graph exact
// 0 / {'source_family': 'keep', 'candidate': 'baseline'} / 7750 / 628178f0823444cb / 961908a478c1be46 / 234:234 / 233,1,0,0 / True
// 1 / {'source_family': 'flat_index', 'candidate': 'baseline'} / 7549 / 23bb96a796b4eb53 / 615715a9aaae2356 / 234:234 / 189,45,0,0 / True

// AXIS source_family
// BEFORE (complete exact span):
// | #define OVERVIEW_TEXT_WIDGET(row, item)                                                            \
// |     (*(textWidgetDynamic + (row) * OVERVIEW_DYNAMIC_WIDGETS_PER_ROW + (item)))
// | #define OVERVIEW_ICON_WIDGET(row, item)                                                            \
// |     (*(iconWidgetDynamic + (row) * OVERVIEW_DYNAMIC_WIDGETS_PER_ROW + (item)))
// ALTERNATIVE flat_index (unified source delta from BEFORE):
// | --- before
// | +++ flat_index
// | @@ -1,4 +1,4 @@
// |  #define OVERVIEW_TEXT_WIDGET(row, item)                                                            \
// | -    (*(textWidgetDynamic + (row) * OVERVIEW_DYNAMIC_WIDGETS_PER_ROW + (item)))
// | +    (textWidgetDynamic[(row) * OVERVIEW_DYNAMIC_WIDGETS_PER_ROW + (item)])
// |  #define OVERVIEW_ICON_WIDGET(row, item)                                                            \
// | -    (*(iconWidgetDynamic + (row) * OVERVIEW_DYNAMIC_WIDGETS_PER_ROW + (item)))
// | +    (iconWidgetDynamic[(row) * OVERVIEW_DYNAMIC_WIDGETS_PER_ROW + (item)])
