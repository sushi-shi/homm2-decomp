// VC6 SP5 Overview source family a04-overview-cleanup; parent ee5ee1cf.
// Source record only. All alternatives below are relative to the stated BEFORE,
// not cumulative diffs. No cross-family/TU-state census is claimed.
// build/source-variant-batch/a04-overview-cleanup/results.json; RVA 0x79ba0
// Complete 2 arms; elapsed 1.940868s; input restored.
// Disposition: retain original; every tested alternative changes text.
// EH private symbol hashes may differ; native comparison canonicalizes actual section owners.
// trial / choices / bytes / text hash / normalized reloc hash / blocks / exact,size,shift,flow / graph exact
// 0 / {'source_family': 'keep', 'candidate': 'baseline'} / 7750 / 628178f0823444cb / 961908a478c1be46 / 234:234 / 233,1,0,0 / True
// 1 / {'source_family': 'flat_index', 'candidate': 'baseline'} / 7727 / 916e92044953591c / 4b40e74e2d069774 / 234:234 / 227,7,0,0 / True

// AXIS source_family
// BEFORE (complete exact span):
// |         for (i = 0; i < OVERVIEW_DYNAMIC_WIDGETS_PER_ROW; i++) {
// |             if (*(textWidgetDynamic + rowIndex * OVERVIEW_DYNAMIC_WIDGETS_PER_ROW + i) != NULL) {
// |                 overWin->RemoveWidget(
// |                     *(textWidgetDynamic + rowIndex * OVERVIEW_DYNAMIC_WIDGETS_PER_ROW + i)
// |                 );
// |                 delete *(textWidgetDynamic + rowIndex * OVERVIEW_DYNAMIC_WIDGETS_PER_ROW + i);
// |                 *(textWidgetDynamic + rowIndex * OVERVIEW_DYNAMIC_WIDGETS_PER_ROW + i) = NULL;
// |             }
// |             if (*(iconWidgetDynamic + rowIndex * OVERVIEW_DYNAMIC_WIDGETS_PER_ROW + i) != NULL) {
// |                 overWin->RemoveWidget(
// |                     *(iconWidgetDynamic + rowIndex * OVERVIEW_DYNAMIC_WIDGETS_PER_ROW + i)
// |                 );
// |                 delete *(iconWidgetDynamic + rowIndex * OVERVIEW_DYNAMIC_WIDGETS_PER_ROW + i);
// |                 *(iconWidgetDynamic + rowIndex * OVERVIEW_DYNAMIC_WIDGETS_PER_ROW + i) = NULL;
// |             }
// ALTERNATIVE flat_index (unified source delta from BEFORE):
// | --- before
// | +++ flat_index
// | @@ -1,15 +1,15 @@
// |          for (i = 0; i < OVERVIEW_DYNAMIC_WIDGETS_PER_ROW; i++) {
// | -            if (*(textWidgetDynamic + rowIndex * OVERVIEW_DYNAMIC_WIDGETS_PER_ROW + i) != NULL) {
// | +            if (textWidgetDynamic[rowIndex * OVERVIEW_DYNAMIC_WIDGETS_PER_ROW + i] != NULL) {
// |                  overWin->RemoveWidget(
// | -                    *(textWidgetDynamic + rowIndex * OVERVIEW_DYNAMIC_WIDGETS_PER_ROW + i)
// | +                    textWidgetDynamic[rowIndex * OVERVIEW_DYNAMIC_WIDGETS_PER_ROW + i]
// |                  );
// | -                delete *(textWidgetDynamic + rowIndex * OVERVIEW_DYNAMIC_WIDGETS_PER_ROW + i);
// | -                *(textWidgetDynamic + rowIndex * OVERVIEW_DYNAMIC_WIDGETS_PER_ROW + i) = NULL;
// | +                delete textWidgetDynamic[rowIndex * OVERVIEW_DYNAMIC_WIDGETS_PER_ROW + i];
// | +                textWidgetDynamic[rowIndex * OVERVIEW_DYNAMIC_WIDGETS_PER_ROW + i] = NULL;
// |              }
// | -            if (*(iconWidgetDynamic + rowIndex * OVERVIEW_DYNAMIC_WIDGETS_PER_ROW + i) != NULL) {
// | +            if (iconWidgetDynamic[rowIndex * OVERVIEW_DYNAMIC_WIDGETS_PER_ROW + i] != NULL) {
// |                  overWin->RemoveWidget(
// | -                    *(iconWidgetDynamic + rowIndex * OVERVIEW_DYNAMIC_WIDGETS_PER_ROW + i)
// | +                    iconWidgetDynamic[rowIndex * OVERVIEW_DYNAMIC_WIDGETS_PER_ROW + i]
// |                  );
// | -                delete *(iconWidgetDynamic + rowIndex * OVERVIEW_DYNAMIC_WIDGETS_PER_ROW + i);
// | -                *(iconWidgetDynamic + rowIndex * OVERVIEW_DYNAMIC_WIDGETS_PER_ROW + i) = NULL;
// | +                delete iconWidgetDynamic[rowIndex * OVERVIEW_DYNAMIC_WIDGETS_PER_ROW + i];
// | +                iconWidgetDynamic[rowIndex * OVERVIEW_DYNAMIC_WIDGETS_PER_ROW + i] = NULL;
// |              }
