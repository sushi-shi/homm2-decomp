// VC6 SP5 kbwin source family s67-kbwin-menu-slots; parentf0502d36.
// Source record only. All alternatives below are relative to the stated BEFORE,
// not cumulative diffs. No cross-family/TU-state census is claimed.
// build/source-variant-batch/s67-kbwin-menu-slots/results.json; RVA 0x71c20
// Complete 4 arms; elapsed 2.389117s; input restored.
// Disposition: retain original; every tested alternative changes text.
// No EH/numerical exceptions in this TU; all sixteen retail checks are complete.
// trial / choices / bytes / text hash / normalized reloc hash / blocks / exact,size,shift,flow / graph exact
// 0 / {'source_family': 'keep', 'candidate': 'baseline'} / 177 / 55d975410d8e0b19 / a8016a34f84281d8 / 13:13 / 13,0,0,0 / True
// 1 / {'source_family': 'value', 'candidate': 'baseline'} / 177 / 166e6dcda13134f8 / a8016a34f84281d8 / 13:13 / 13,0,0,0 / True
// 2 / {'source_family': 'result', 'candidate': 'baseline'} / 177 / 166e6dcda13134f8 / a8016a34f84281d8 / 13:13 / 13,0,0,0 / True
// 3 / {'source_family': 'result_value', 'candidate': 'baseline'} / 175 / d34dbaffa505abb8 / 8f957661ac3c133f / 13:13 / 13,0,0,0 / True

// AXIS source_family
// BEFORE (complete exact span):
// | void UpdateDfltMenu(HMENU menu) {
// |     i32 H2_UNUSED(result);
// |     i32 H2_UNUSED(value);
// |
// |     if (gConfig.gfx[IDX(giCurExe)].showMenu == 0)
// |         return;
// |     if (giMainVideoModeWidth <= KBWIN_WIDTH_640)
// |         EnableMenuItem(menu, IDX(KBWIN_MENU_SIZE_640_480), MF_GRAYED);
// |     if (giMainVideoModeWidth <= KBWIN_WIDTH_800)
// |         EnableMenuItem(menu, IDX(KBWIN_MENU_SIZE_800_600), MF_GRAYED);
// |     if (giMainVideoModeWidth <= KBWIN_WIDTH_1024)
// |         EnableMenuItem(menu, IDX(KBWIN_MENU_SIZE_1024_768), MF_GRAYED);
// |     if (giMainVideoModeWidth <= KBWIN_WIDTH_1280)
// |         EnableMenuItem(menu, IDX(KBWIN_MENU_SIZE_1280_1024), MF_GRAYED);
// |     if (gbDDrawAttached == 0)
// |         EnableMenuItem(menu, IDX(KBWIN_MENU_FULLSCREEN), MF_GRAYED);
// | }
// ALTERNATIVE value (unified source delta from BEFORE):
// | --- before
// | +++ value
// | @@ -1,6 +1,5 @@
// |  void UpdateDfltMenu(HMENU menu) {
// |      i32 H2_UNUSED(result);
// | -    i32 H2_UNUSED(value);
// |
// |      if (gConfig.gfx[IDX(giCurExe)].showMenu == 0)
// |          return;
// ALTERNATIVE result (unified source delta from BEFORE):
// | --- before
// | +++ result
// | @@ -1,5 +1,4 @@
// |  void UpdateDfltMenu(HMENU menu) {
// | -    i32 H2_UNUSED(result);
// |      i32 H2_UNUSED(value);
// |
// |      if (gConfig.gfx[IDX(giCurExe)].showMenu == 0)
// ALTERNATIVE result_value (unified source delta from BEFORE):
// | --- before
// | +++ result_value
// | @@ -1,6 +1,4 @@
// |  void UpdateDfltMenu(HMENU menu) {
// | -    i32 H2_UNUSED(result);
// | -    i32 H2_UNUSED(value);
// |
// |      if (gConfig.gfx[IDX(giCurExe)].showMenu == 0)
// |          return;
