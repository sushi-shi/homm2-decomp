// VC6 SP5 Overview source family c72-overview-params-row; parent ee5ee1cf.
// Source record only. All alternatives below are relative to the stated BEFORE,
// not cumulative diffs. No cross-family/TU-state census is claimed.
// build/source-variant-batch/c72-overview-params-row/results.json; RVA 0x79ba0
// Complete 2 arms; elapsed 1.936598s; input restored.
// Disposition: retain this family's cast removals; combined all98 native proof.
// EH private symbol hashes may differ; native comparison canonicalizes actual section owners.
// trial / choices / bytes / text hash / normalized reloc hash / blocks / exact,size,shift,flow / graph exact
// 0 / {'source_family': 'keep', 'candidate': 'baseline'} / 7750 / 628178f0823444cb / 961908a478c1be46 / 234:234 / 233,1,0,0 / True
// 1 / {'source_family': 'implicit_parameters', 'candidate': 'baseline'} / 7750 / 628178f0823444cb / 5c3cf27607e76ef5 / 234:234 / 233,1,0,0 / True

// AXIS source_family
// BEFORE (complete exact span):
// |             ROW_BACKGROUND_X,
// |             static_cast<i16>(rowIndex * OVERVIEW_ROW_HEIGHT + ROW_BACKGROUND_Y_OFFSET),
// |             ROW_BACKGROUND_WIDTH,
// |             ROW_BACKGROUND_HEIGHT,
// |             const_cast<char*>("overview.icn"),
// |             static_cast<i16>(IDX(giOverviewType) + ROW_BACKGROUND_FRAME_BASE),
// |             ICON_DRAW_NORMAL,
// |             static_cast<i16>(rowWidgetId + ROW_BACKGROUND_CONTROL),
// |             WIDGET_KIND_ICON_DIRECT,
// ALTERNATIVE implicit_parameters (unified source delta from BEFORE):
// | --- before
// | +++ implicit_parameters
// | @@ -1,9 +1,9 @@
// |              ROW_BACKGROUND_X,
// | -            static_cast<i16>(rowIndex * OVERVIEW_ROW_HEIGHT + ROW_BACKGROUND_Y_OFFSET),
// | +            rowIndex * OVERVIEW_ROW_HEIGHT + ROW_BACKGROUND_Y_OFFSET,
// |              ROW_BACKGROUND_WIDTH,
// |              ROW_BACKGROUND_HEIGHT,
// |              const_cast<char*>("overview.icn"),
// | -            static_cast<i16>(IDX(giOverviewType) + ROW_BACKGROUND_FRAME_BASE),
// | +            IDX(giOverviewType) + ROW_BACKGROUND_FRAME_BASE,
// |              ICON_DRAW_NORMAL,
// | -            static_cast<i16>(rowWidgetId + ROW_BACKGROUND_CONTROL),
// | +            rowWidgetId + ROW_BACKGROUND_CONTROL,
// |              WIDGET_KIND_ICON_DIRECT,
