// VC6 SP5 Overview source family a04-overview-initialize; parent ee5ee1cf.
// Source record only. All alternatives below are relative to the stated BEFORE,
// not cumulative diffs. No cross-family/TU-state census is claimed.
// build/source-variant-batch/a04-overview-initialize/results.json; RVA 0x7bda1
// Complete 2 arms; elapsed 1.919081s; input restored.
// Disposition: retain original; every tested alternative changes text.
// EH private symbol hashes may differ; native comparison canonicalizes actual section owners.
// trial / choices / bytes / text hash / normalized reloc hash / blocks / exact,size,shift,flow / graph exact
// 0 / {'source_family': 'keep', 'candidate': 'baseline'} / 1202 / 688689bd5599e522 / 9cebe67a573ac9ef / 42:42 / 42,0,0,0 / True
// 1 / {'source_family': 'flat_index', 'candidate': 'baseline'} / 1197 / 580e53b6d87d0204 / 4f47e372b09c1bdd / 42:42 / 41,1,0,0 / True

// AXIS source_family
// BEFORE (complete exact span):
// |         for (mine = 0; mine < OVERVIEW_DYNAMIC_WIDGETS_PER_ROW; mine++) {
// |             *(textWidgetDynamic + y * OVERVIEW_DYNAMIC_WIDGETS_PER_ROW + mine) = NULL;
// |             *(iconWidgetDynamic + y * OVERVIEW_DYNAMIC_WIDGETS_PER_ROW + mine) = NULL;
// |         }
// ALTERNATIVE flat_index (unified source delta from BEFORE):
// | --- before
// | +++ flat_index
// | @@ -1,4 +1,4 @@
// |          for (mine = 0; mine < OVERVIEW_DYNAMIC_WIDGETS_PER_ROW; mine++) {
// | -            *(textWidgetDynamic + y * OVERVIEW_DYNAMIC_WIDGETS_PER_ROW + mine) = NULL;
// | -            *(iconWidgetDynamic + y * OVERVIEW_DYNAMIC_WIDGETS_PER_ROW + mine) = NULL;
// | +            textWidgetDynamic[y * OVERVIEW_DYNAMIC_WIDGETS_PER_ROW + mine] = NULL;
// | +            iconWidgetDynamic[y * OVERVIEW_DYNAMIC_WIDGETS_PER_ROW + mine] = NULL;
// |          }
