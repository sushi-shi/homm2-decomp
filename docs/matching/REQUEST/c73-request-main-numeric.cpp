// VC6 SP5 REQUEST source family c73-request-main-numeric; parent13e6af40.
// Source record only. All alternatives below are relative to the stated BEFORE,
// not cumulative diffs. No cross-family/TU-state census is claimed.
// build/source-variant-batch/c73-request-main-numeric/results.json; RVA 0x8f737
// Complete 4 arms; elapsed 2.904818s; input restored.
// Disposition: retain all scalar removals in this family (nine-character scratch for S66); combined all98 native proof.
// EH private symbol hashes may differ; native comparison canonicalizes actual section owners.
// trial / choices / bytes / text hash / normalized reloc hash / blocks / exact,size,shift,flow / graph exact
// 0 / {'source_family': 'keep', 'candidate': 'baseline'} / 4388 / a96d6354363d97b7 / b5daded3af5efd4c / 195:196 / 194,0,0,1 / False
// 1 / {'source_family': 'number1', 'candidate': 'baseline'} / 4388 / a96d6354363d97b7 / 976c2b651eca4aca / 195:196 / 194,0,0,1 / False
// 2 / {'source_family': 'number0', 'candidate': 'baseline'} / 4388 / a96d6354363d97b7 / 976c2b651eca4aca / 195:196 / 194,0,0,1 / False
// 3 / {'source_family': 'number0_number1', 'candidate': 'baseline'} / 4388 / a96d6354363d97b7 / 6fa448395d3ed977 / 195:196 / 194,0,0,1 / False

// AXIS source_family
// BEFORE (complete exact span):
// |                                     positions = 1;
// |                                 stepScreen = static_cast<i32>(
// |                                     (fGutterTravelLength
// |                                      * IDX(FILE_REQUESTER_GUTTER_SCALE))
// |                                     / positions
// |                                 );
// |                                 mouseX = message.payload.widget.screenX;
// |                                 screenY = message.payload.widget.screenY;
// |                                 screenY = static_cast<i32>(screenY - (m_y + fGutterMinY));
// |                                 screenY -= FILE_REQUESTER_SCROLL_KNOB_HALF_HEIGHT;
// ALTERNATIVE number1 (unified source delta from BEFORE):
// | --- before
// | +++ number1
// | @@ -6,5 +6,5 @@
// |                                  );
// |                                  mouseX = message.payload.widget.screenX;
// |                                  screenY = message.payload.widget.screenY;
// | -                                screenY = static_cast<i32>(screenY - (m_y + fGutterMinY));
// | +                                screenY = (screenY - (m_y + fGutterMinY));
// |                                  screenY -= FILE_REQUESTER_SCROLL_KNOB_HALF_HEIGHT;
// ALTERNATIVE number0 (unified source delta from BEFORE):
// | --- before
// | +++ number0
// | @@ -1,9 +1,5 @@
// |                                      positions = 1;
// | -                                stepScreen = static_cast<i32>(
// | -                                    (fGutterTravelLength
// | -                                     * IDX(FILE_REQUESTER_GUTTER_SCALE))
// | -                                    / positions
// | -                                );
// | +                                stepScreen = ((fGutterTravelLength * IDX(FILE_REQUESTER_GUTTER_SCALE)) / positions);
// |                                  mouseX = message.payload.widget.screenX;
// |                                  screenY = message.payload.widget.screenY;
// |                                  screenY = static_cast<i32>(screenY - (m_y + fGutterMinY));
// ALTERNATIVE number0_number1 (unified source delta from BEFORE):
// | --- before
// | +++ number0_number1
// | @@ -1,10 +1,6 @@
// |                                      positions = 1;
// | -                                stepScreen = static_cast<i32>(
// | -                                    (fGutterTravelLength
// | -                                     * IDX(FILE_REQUESTER_GUTTER_SCALE))
// | -                                    / positions
// | -                                );
// | +                                stepScreen = ((fGutterTravelLength * IDX(FILE_REQUESTER_GUTTER_SCALE)) / positions);
// |                                  mouseX = message.payload.widget.screenX;
// |                                  screenY = message.payload.widget.screenY;
// | -                                screenY = static_cast<i32>(screenY - (m_y + fGutterMinY));
// | +                                screenY = (screenY - (m_y + fGutterMinY));
// |                                  screenY -= FILE_REQUESTER_SCROLL_KNOB_HALF_HEIGHT;
