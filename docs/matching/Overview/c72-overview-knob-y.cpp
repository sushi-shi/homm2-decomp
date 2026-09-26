// VC6 SP5 Overview source family c72-overview-knob-y; parent ee5ee1cf.
// Source record only. All alternatives below are relative to the stated BEFORE,
// not cumulative diffs. No cross-family/TU-state census is claimed.
// build/source-variant-batch/c72-overview-knob-y/results.json; RVA 0x7c253
// Complete 2 arms; elapsed 1.737255s; input restored.
// Disposition: retain this family's cast removals; combined all98 native proof.
// EH private symbol hashes may differ; native comparison canonicalizes actual section owners.
// trial / choices / bytes / text hash / normalized reloc hash / blocks / exact,size,shift,flow / graph exact
// 0 / {'source_family': 'keep', 'candidate': 'baseline'} / 686 / bca9984e0fd80ef9 / a3aa3dc9463b4ea9 / 27:27 / 27,0,0,0 / True
// 1 / {'source_family': 'y_assignments', 'candidate': 'baseline'} / 686 / bca9984e0fd80ef9 / a3aa3dc9463b4ea9 / 27:27 / 27,0,0,0 / True

// AXIS source_family
// BEFORE (complete exact span):
// |                 OVScrollKnob->m_y =
// |                     static_cast<i16>(widgetMessage.payload.mouse.y - SCROLL_KNOB_OFFSET);
// |                 topNow = static_cast<i32>((OVScrollKnob->m_y - topValue) / itemPixels);
// |                 if (topNow != topBefore) {
// |                     if (topNow > giOverviewItems[IDX(giOverviewType)] - OVERVIEW_VISIBLE_ROWS) {
// |                         topNow = giOverviewItems[IDX(giOverviewType)] - OVERVIEW_VISIBLE_ROWS;
// |                     }
// |                     if (topNow < 0) {
// |                         topNow = 0;
// |                     }
// |                     giOverviewTop[IDX(giOverviewType)] = topNow;
// |                     OVScrollKnob->m_y =
// |                         static_cast<i16>(widgetMessage.payload.mouse.y - SCROLL_KNOB_OFFSET);
// |                     SetupDynamicStuff(1, 0, 0);
// ALTERNATIVE y_assignments (unified source delta from BEFORE):
// | --- before
// | +++ y_assignments
// | @@ -1,5 +1,5 @@
// |                  OVScrollKnob->m_y =
// | -                    static_cast<i16>(widgetMessage.payload.mouse.y - SCROLL_KNOB_OFFSET);
// | +                    widgetMessage.payload.mouse.y - SCROLL_KNOB_OFFSET;
// |                  topNow = static_cast<i32>((OVScrollKnob->m_y - topValue) / itemPixels);
// |                  if (topNow != topBefore) {
// |                      if (topNow > giOverviewItems[IDX(giOverviewType)] - OVERVIEW_VISIBLE_ROWS) {
// | @@ -10,5 +10,5 @@
// |                      }
// |                      giOverviewTop[IDX(giOverviewType)] = topNow;
// |                      OVScrollKnob->m_y =
// | -                        static_cast<i16>(widgetMessage.payload.mouse.y - SCROLL_KNOB_OFFSET);
// | +                        widgetMessage.payload.mouse.y - SCROLL_KNOB_OFFSET;
// |                      SetupDynamicStuff(1, 0, 0);
