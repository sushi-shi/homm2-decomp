// VC6 SP5 source family c76-newgame-init-widget6; parent f84e0b7c plus U18 incoming packet header.
// Except O07 itself, parent includes O07 recovered outgoing300-byte owner.
// Source record only: alternatives are relative to BEFORE, not cumulative.
// No cross-family Cartesian product or TU-state search is claimed.
// build/source-variant-batch/c76-newgame-init-widget6/results.json; RVA 0x766a7
// Complete 4 arms; elapsed 3.137454s; source restored.
// Disposition: retain all tested destination/promotion cast removals; map domain keeps inner i16 truncation and uses enum constructor.
// Private label/SG/EH counters can change relocation hashes without changing destinations.
// Final native objects/retail sites and targets decide retention, not fuzzy scores.
// Existing four EH/FS model gaps remain open; reviewed _stricmp alias is pinned separately.
// trial / choices / bytes / text hash / normalized reloc hash / blocks / exact,size,shift,flow / graph exact
// 0 / {'family': 'keep', 'candidate': 'baseline'} / 1940 / a82616cb79137c3d / 5bcb53de2af7ca8e / 62:62 / 62,0,0,0 / True
// 1 / {'family': 'remove_01', 'candidate': 'baseline'} / 1940 / a82616cb79137c3d / b2891d20997c2734 / 62:62 / 62,0,0,0 / True
// 2 / {'family': 'remove_10', 'candidate': 'baseline'} / 1940 / a82616cb79137c3d / b2891d20997c2734 / 62:62 / 62,0,0,0 / True
// 3 / {'family': 'remove_11', 'candidate': 'baseline'} / 1940 / a82616cb79137c3d / 1cde1fcbd2bf0603 / 62:62 / 62,0,0,0 / True

// AXIS family
// BEFORE (complete exact span):
// | new textWidget(
// |                     static_cast<i16>(
// |                         firstColumnX + playerStep * playerCounter + PLAYER_NAME_X_OFFSET
// |                     ),
// |                     PLAYER_NAME_Y,
// |                     PLAYER_NAME_WIDTH,
// |                     PLAYER_NAME_HEIGHT,
// |                     name,
// |                     "smalfont.fnt",
// |                     FONT_DRAW_DEFAULT,
// |                     static_cast<i16>(playerCounter + NEW_GAME_PLAYER_NAME_FIRST),
// |                     WIDGET_KIND_TEXT,
// |                     FONT_ALIGN_CENTER
// |                 );
// ALTERNATIVE remove_01 (unified delta from BEFORE):
// | --- before
// | +++ remove_01
// | @@ -8,7 +8,7 @@
// |                      name,
// |                      "smalfont.fnt",
// |                      FONT_DRAW_DEFAULT,
// | -                    static_cast<i16>(playerCounter + NEW_GAME_PLAYER_NAME_FIRST),
// | +                    playerCounter + NEW_GAME_PLAYER_NAME_FIRST,
// |                      WIDGET_KIND_TEXT,
// |                      FONT_ALIGN_CENTER
// |                  );
// ALTERNATIVE remove_10 (unified delta from BEFORE):
// | --- before
// | +++ remove_10
// | @@ -1,7 +1,5 @@
// |  new textWidget(
// | -                    static_cast<i16>(
// | -                        firstColumnX + playerStep * playerCounter + PLAYER_NAME_X_OFFSET
// | -                    ),
// | +                    firstColumnX + playerStep * playerCounter + PLAYER_NAME_X_OFFSET,
// |                      PLAYER_NAME_Y,
// |                      PLAYER_NAME_WIDTH,
// |                      PLAYER_NAME_HEIGHT,
// ALTERNATIVE remove_11 (unified delta from BEFORE):
// | --- before
// | +++ remove_11
// | @@ -1,14 +1,12 @@
// |  new textWidget(
// | -                    static_cast<i16>(
// | -                        firstColumnX + playerStep * playerCounter + PLAYER_NAME_X_OFFSET
// | -                    ),
// | +                    firstColumnX + playerStep * playerCounter + PLAYER_NAME_X_OFFSET,
// |                      PLAYER_NAME_Y,
// |                      PLAYER_NAME_WIDTH,
// |                      PLAYER_NAME_HEIGHT,
// |                      name,
// |                      "smalfont.fnt",
// |                      FONT_DRAW_DEFAULT,
// | -                    static_cast<i16>(playerCounter + NEW_GAME_PLAYER_NAME_FIRST),
// | +                    playerCounter + NEW_GAME_PLAYER_NAME_FIRST,
// |                      WIDGET_KIND_TEXT,
// |                      FONT_ALIGN_CENTER
// |                  );
